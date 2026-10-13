#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""growth_core.py — 成长数据的计算内核（供渲染器与 AI 接口共用）。

职责边界
--------
- 只做：读取真值源 JSON → 校验 → 计算派生指标 → 汇总快照 / 生成给 AI 的摘要。
- 不做：任何 HTML / SVG 渲染（那是 build_dashboard.py 的事）。

唯一真值源：`真实比赛/比赛记录/*.json`。本模块只读它，绝不回写。
"""

import json
from collections import OrderedDict
from datetime import datetime
from pathlib import Path

# --------------------------------------------------------------------------- #
# 常量（改这里即可全局生效）
# --------------------------------------------------------------------------- #

REPO_ROOT = Path(__file__).resolve().parent.parent
RECORDS_DIR = REPO_ROOT / "真实比赛" / "比赛记录"
TRAINING_DIR = REPO_ROOT / "训练记录" / "日志"
DATA_DIR = REPO_ROOT / "成长数据"

STOP_LOSS_LINE_MINUTES = 25   # 赛前写死的单题止损线（分钟）
REACHABLE_RATE_FLOOR = 0.30   # 期望分失分只统计主通过率 >= 30% 的题
TREND_MIN_CONTESTS = 4        # 少于这么多场，禁止跨场趋势线
REG_COEF_MIN_CONTESTS = 5     # 少于这么多场，禁止回归 / 预测
RADAR_MIN_CONTESTS = 3        # 少于这么多场，禁止雷达面积对比
TRAINING_TREND_MIN_DAYS = 4   # 训练日少于这么多天，禁止训练趋势线
TIER_BOUNDS = [("签到", 0.90), ("简单", 0.60), ("中档", 0.30), ("困难", 0.0)]
TIER_BORDER_MARGIN = 0.02     # 距档位边界 2pp 内标记为「边界」

INDEPENDENT_VALUES = ("独立", "卡住后自己过", "看题解后过")  # 有完成度标注 = 参与独立AC率
EVIDENCE_LEVELS = ("强", "中", "弱")

SCHEMA_VERSION = "1.0"


class GrowthError(Exception):
    """数据校验失败。"""


# --------------------------------------------------------------------------- #
# 读取与校验
# --------------------------------------------------------------------------- #


def load_contests(records_dir=RECORDS_DIR):
    if not records_dir.is_dir():
        raise GrowthError(f"找不到比赛记录目录：{records_dir}")
    files = sorted(p for p in records_dir.glob("*.json") if not p.name.startswith("_"))
    if not files:
        raise GrowthError(f"{records_dir} 下没有比赛 JSON 文件")
    contests = []
    for p in files:
        try:
            with p.open(encoding="utf-8") as fh:
                data = json.load(fh)
        except json.JSONDecodeError as e:
            raise GrowthError(f"{p.name} 不是合法 JSON：{e}")
        validate(data, p.name)
        data["_source_file"] = p.name
        contests.append(data)
    contests.sort(key=lambda c: c["contest"]["date"])
    return contests


def validate(c, fname):
    """轻量校验：只查会导致误算的关键字段，缺则报错并指出键路径。"""
    for k in ("schema_version", "contest", "scoring", "problems", "provenance"):
        if k not in c:
            raise GrowthError(f"{fname}: 缺少顶层键 '{k}'")
    if not c["problems"]:
        raise GrowthError(f"{fname}: problems 为空")
    for i, p in enumerate(c["problems"]):
        for k in ("letter", "full_score", "pass_rates", "my_score", "status"):
            if k not in p:
                raise GrowthError(f"{fname}: problems[{i}] 缺少键 '{k}'")
        pr = p["pass_rates"]
        src = pr.get("primary_source")
        if src is None:
            raise GrowthError(f"{fname}: 题 {p['letter']} 缺少 pass_rates.primary_source")
        try:
            key = {"封榜": "frozen", "官方终版": "official_final", "官方当场": "official_insession"}[src]
        except KeyError:
            raise GrowthError(f"{fname}: 题 {p['letter']} 的 primary_source='{src}' 非法")
        if pr.get(key) is None:
            raise GrowthError(f"{fname}: 题 {p['letter']} 的 primary_source='{src}'，但 {key} 为 null")


def check_all(records_dir=RECORDS_DIR):
    """返回 (警告列表, 错误列表)，供 `check` 子命令使用，不抛异常。"""
    warnings, errors = [], []
    try:
        files = sorted(p for p in records_dir.glob("*.json") if not p.name.startswith("_"))
    except OSError as e:
        return [], [f"读取目录失败：{e}"]
    if not files:
        return [], [f"{records_dir} 下没有比赛 JSON 文件"]
    for p in files:
        try:
            with p.open(encoding="utf-8") as fh:
                data = json.load(fh)
            validate(data, p.name)
        except GrowthError as e:
            errors.append(str(e))
            continue
        except json.JSONDecodeError as e:
            errors.append(f"{p.name} 不是合法 JSON：{e}")
            continue
        if data.get("timeline_reconstructed"):
            warnings.append(f"{p.name}: timeline 为重建值（原始复盘无绝对起止时刻）")
        for pr in data["problems"]:
            if pr["status"] == "full" and abs(pr["my_score"] - pr["full_score"]) > 1e-9:
                warnings.append(f"{p.name}: 题 {pr['letter']} status=full 但 my_score != full_score")
            if pr.get("post_contest_score") and pr.get("post_contest_counts"):
                warnings.append(f"{p.name}: 题 {pr['letter']} 的赛后补题分被标记为计入总分——请确认（默认不计入）")
        sc = data["scoring"]
        if sc.get("rank") is not None and sc.get("participants") is None:
            warnings.append(f"{p.name}: 有名次但缺总人数 → 百分位将显示为「数据缺失」")
    tw, te = check_training()
    warnings.extend(tw)
    errors.extend(te)
    return warnings, errors


# --------------------------------------------------------------------------- #
# 计算派生量
# --------------------------------------------------------------------------- #


def _rate_of(node):
    """优先用 numerator/denominator 现算，缺失才退回手写的 rate（避免舍入误差从 JSON 带进指标）。"""
    if not node:
        return None
    num, den = node.get("numerator"), node.get("denominator")
    if num is not None and den:
        return num / den
    return node.get("rate")


def primary_rate(p):
    pr = p["pass_rates"]
    key = {"封榜": "frozen", "官方终版": "official_final", "官方当场": "official_insession"}[pr["primary_source"]]
    return _rate_of(pr[key])


def tier_rate(p):
    """难度档一律用封榜通过率；无封榜则退到官方当场；再无则官方终版。"""
    pr = p["pass_rates"]
    for k in ("frozen", "official_insession", "official_final"):
        r = _rate_of(pr.get(k))
        if r is not None:
            return r
    return None


def classify_tier(rate):
    if rate is None:
        return None, False
    tier = TIER_BOUNDS[-1][0]
    for name, lo in TIER_BOUNDS:
        if rate >= lo:
            tier = name
            break
    border = any(abs(rate - lo) <= TIER_BORDER_MARGIN for _, lo in TIER_BOUNDS[1:])
    return tier, border


def attempted(p):
    if p["status"] != "unattempted":
        return True
    t = p.get("time_spent_minutes")
    return bool(t and t > 0)


def enrich(c):
    e = dict(c)
    probs = []
    for p in c["problems"]:
        q = dict(p)
        r = primary_rate(p)
        q["_primary_rate"] = r
        q["expected_score"] = round(r * p["full_score"], 1)
        q["my_score_rate"] = round(p["my_score"] / p["full_score"], 4) if p["full_score"] else 0.0
        q["attempted"] = attempted(p)
        tr = tier_rate(p)
        q["tier"], q["tier_border"] = classify_tier(tr)
        q["_tier_rate"] = tr
        probs.append(q)
    e["problems"] = probs

    sc = c["scoring"]
    e["solved_full_count"] = sum(1 for p in probs if p["status"] == "full")
    e["partial_count"] = sum(1 for p in probs if p["status"] == "partial")
    e["score_rate"] = (sc["my_total"] / sc["total_possible"]) if sc.get("total_possible") else None
    e["percentile"] = (sc["rank"] / sc["participants"]) if sc.get("rank") and sc.get("participants") else None

    e["expected_ordering"] = [p["letter"] for p in sorted(probs, key=lambda x: -x["expected_score"])]

    actual = [s["stage"] for s in c.get("timeline", []) if s.get("kind") == "题"]
    e["actual_ordering"] = actual

    nontrivial = [p for p in sorted(probs, key=lambda x: -x["expected_score"]) if p["tier"] != "签到"]
    want = [p["letter"] for p in nontrivial]
    got = set(p["letter"] for p in nontrivial if p["attempted"])
    prefix = []
    for L in want:
        if L in got:
            prefix.append(L)
        else:
            break
    e["ordering_followed"] = (len(prefix) == len(got))
    e["ordering_want"] = want
    e["ordering_got"] = [L for L in want if L in got]

    violations = []
    for p in probs:
        t = p.get("time_spent_minutes")
        if t and t > STOP_LOSS_LINE_MINUTES and p["status"] != "full":
            violations.append({"letter": p["letter"], "minutes": t})
    e["stop_loss_violations"] = violations
    e["stop_loss_executed"] = (len(violations) == 0)

    zero = [p for p in probs if p["my_score"] == 0 and p["expected_score"] > 0]
    if zero:
        e["missing_highest_expected"] = max(zero, key=lambda x: x["expected_score"])
    else:
        notfull = [p for p in probs if p["status"] != "full"]
        e["missing_highest_expected"] = max(notfull, key=lambda x: x["expected_score"]) if notfull else None

    e["idle_tail_minutes"] = round(sum(s["minutes"] for s in c.get("timeline", []) if s.get("kind") == "空转"), 2)

    e["expected_loss_reachable"] = round(sum(
        max(0.0, p["expected_score"] - p["my_score"])
        for p in probs if p["_primary_rate"] >= REACHABLE_RATE_FLOOR
    ), 1)

    tiers = OrderedDict((t, {"total": 0, "full": 0, "attempted": 0}) for t, _ in TIER_BOUNDS)
    for p in probs:
        if p["tier"] is None:
            continue
        tiers[p["tier"]]["total"] += 1
        if p["status"] == "full":
            tiers[p["tier"]]["full"] += 1
        if p["attempted"]:
            tiers[p["tier"]]["attempted"] += 1
    e["tiers"] = tiers
    e["midtier_closed"] = tiers.get("中档", {"full": 0})["full"]
    e["midtier_total"] = tiers.get("中档", {"total": 0})["total"]

    e["total_minutes_used"] = round(sum(p["time_spent_minutes"] or 0 for p in probs), 1)
    e["score_per_minute"] = (sc["my_total"] / e["total_minutes_used"]) if e["total_minutes_used"] else None
    return e


def enrich_all(contests):
    return [enrich(c) for c in contests]


# --------------------------------------------------------------------------- #
# 训练记录（第二个真值源）
# --------------------------------------------------------------------------- #


def load_training(training_dir=TRAINING_DIR):
    """读取训练日志。目录不存在或为空时返回 []（训练层可为空，不影响比赛层）。"""
    if not training_dir.is_dir():
        return []
    files = sorted(p for p in training_dir.glob("*.json") if not p.name.startswith("_"))
    out = []
    for p in files:
        try:
            with p.open(encoding="utf-8") as fh:
                data = json.load(fh)
        except json.JSONDecodeError as e:
            raise GrowthError(f"{p.name} 不是合法 JSON：{e}")
        validate_training(data, p.name)
        data["_source_file"] = p.name
        out.append(data)
    out.sort(key=lambda r: r["date"])
    return out


def validate_training(r, fname):
    for k in ("schema_version", "date", "platform", "evidence_level", "problems"):
        if k not in r:
            raise GrowthError(f"{fname}: 缺少顶层键 '{k}'")
    if r["evidence_level"] not in EVIDENCE_LEVELS:
        raise GrowthError(f"{fname}: evidence_level='{r['evidence_level']}' 非法")
    if not r["problems"]:
        raise GrowthError(f"{fname}: problems 为空")
    for i, p in enumerate(r["problems"]):
        for k in ("id", "verdict"):
            if k not in p:
                raise GrowthError(f"{fname}: problems[{i}] 缺少键 '{k}'")
        ind = p.get("independent")
        if ind is not None and ind not in INDEPENDENT_VALUES:
            raise GrowthError(f"{fname}: 题 {p['id']} 的 independent='{ind}' 非法")
        if r["evidence_level"] == "强" and p["verdict"] == "AC" and not p.get("code_file"):
            # 强证据必须能对上原始代码，否则降级——这里只告警不报错，由 check 收集
            pass


def check_training(training_dir=TRAINING_DIR):
    warnings, errors = [], []
    if not training_dir.is_dir():
        return warnings, errors
    files = sorted(p for p in training_dir.glob("*.json") if not p.name.startswith("_"))
    for p in files:
        try:
            with p.open(encoding="utf-8") as fh:
                r = json.load(fh)
            validate_training(r, p.name)
        except GrowthError as e:
            errors.append(str(e))
            continue
        except json.JSONDecodeError as e:
            errors.append(f"{p.name} 不是合法 JSON：{e}")
            continue
        lvl = r["evidence_level"]
        for pr in r["problems"]:
            if lvl == "强" and pr["verdict"] == "AC" and not pr.get("code_file"):
                warnings.append(f"{p.name}: 题 {pr['id']} 标为强证据但无 code_file（强证据须能对上原始代码）")
            if pr["verdict"] == "AC" and pr.get("independent") is None and lvl != "弱":
                warnings.append(f"{p.name}: 题 {pr['id']} AC 但未填 independent（无法计入独立AC率）")
        if r.get("date_range") and lvl != "弱":
            warnings.append(f"{p.name}: 使用 date_range 的批量记录但证据等级为「{lvl}」，请确认是否应为弱证据")
    return warnings, errors


def training_metrics(records):
    """把逐日训练记录聚合成指标。核心是「独立 AC 率」——只统计有完成度标注的题。"""
    if not records:
        return None

    problems = [(r, p) for r in records for p in r["problems"]]
    total = len(problems)
    ac = sum(1 for _, p in problems if p["verdict"] == "AC")

    scored = [(r, p) for r, p in problems if p.get("independent") in INDEPENDENT_VALUES]
    n_ind = sum(1 for _, p in scored if p["independent"] == "独立")
    n_hint = sum(1 for _, p in scored if p["independent"] == "卡住后自己过")
    n_dep = sum(1 for _, p in scored if p["independent"] == "看题解后过")

    by_tag = OrderedDict()
    for _, p in scored:
        for t in (p.get("tags") or ["未标注"]):
            d = by_tag.setdefault(t, {"scored": 0, "independent": 0})
            d["scored"] += 1
            if p["independent"] == "独立":
                d["independent"] += 1
    for d in by_tag.values():
        d["independent_rate"] = round(d["independent"] / d["scored"], 4) if d["scored"] else None
    by_tag = OrderedDict(sorted(by_tag.items(), key=lambda kv: (-kv[1]["scored"], kv[0])))

    # 证据分级：按记录数 + 按题数
    ev_records = {lv: 0 for lv in EVIDENCE_LEVELS}
    ev_problems = {lv: 0 for lv in EVIDENCE_LEVELS}
    for r in records:
        ev_records[r["evidence_level"]] += 1
        ev_problems[r["evidence_level"]] += len(r["problems"])

    dates = sorted({r["date"] for r in records})
    streak = _training_streak(dates)

    mins = sorted(p["minutes"] for _, p in problems if p.get("minutes") is not None)
    median_min = mins[len(mins) // 2] if mins else None

    return {
        "days": len(dates),
        "date_first": dates[0] if dates else None,
        "date_last": dates[-1] if dates else None,
        "training_streak_days": streak,
        "problems_total": total,
        "ac_count": ac,
        "scored_count": len(scored),
        "independent_count": n_ind,
        "hinted_count": n_hint,
        "solution_dependent_count": n_dep,
        "independent_ac_rate": round(n_ind / len(scored), 4) if scored else None,
        "dependency_rate": round((n_hint + n_dep) / len(scored), 4) if scored else None,
        "weak_problem_count": ev_problems["弱"],
        "evidence_records": ev_records,
        "evidence_problems": ev_problems,
        "by_tag": by_tag,
        "median_minutes_per_problem": median_min,
        "problems_per_day": round(total / len(dates), 1) if dates else None,
    }


def _training_streak(dates):
    """从最近训练日往前数，日历连续的天数。"""
    if not dates:
        return 0
    try:
        ds = [datetime.strptime(d, "%Y-%m-%d").date() for d in dates]
    except ValueError:
        return 1
    ds.sort()
    streak = 1
    cur = ds[-1]
    for d in reversed(ds[:-1]):
        if (cur - d).days == 1:
            streak += 1
            cur = d
        else:
            break
    return streak


def build_training_block(records):
    m = training_metrics(records)
    if m is None:
        return {
            "status": "empty",
            "message": "训练记录尚未录入。真值源目录：训练记录/日志/",
            "constraints": [],
        }
    m["status"] = "ok"
    m["constraints"] = [
        f"训练日 {m['days']} < {TRAINING_TREND_MIN_DAYS}：禁止画训练趋势线，只能做汇总"
        if m["days"] < TRAINING_TREND_MIN_DAYS else "可画训练趋势线"
    ]
    if m["independent_ac_rate"] is None:
        m["note"] = "尚无任何题带完成度标注 → 独立AC率不可计算。当前训练数据只能证明『做过』。"
    return m


# --------------------------------------------------------------------------- #
# 停滞（streak）分析
# --------------------------------------------------------------------------- #

STREAK_SERIES = [
    ("完全通过题数", "solved_full_count", lambda c: c["solved_full_count"]),
    ("单题止损执行", "stop_loss_executed", lambda c: c["stop_loss_executed"]),
    ("部分分题数", "partial_count", lambda c: c["partial_count"]),
    ("中档题闭环数", "midtier_closed", lambda c: c["midtier_closed"]),
]


def compute_streaks(contests):
    out = []
    for label, key, get in STREAK_SERIES:
        if len(contests) < 2:
            continue
        last = get(contests[-1])
        streak = 1
        for c in reversed(contests[:-1]):
            if get(c) == last:
                streak += 1
            else:
                break
        out.append({
            "label": label, "key": key, "value": last, "streak": streak,
            "prev_value": get(contests[-2]), "stalled": streak >= 2,
        })
    return out


def stagnation_verdict(s):
    label, v = s["label"], s["value"]
    if label == "完全通过题数":
        return f"连续 {s['streak']} 场都是 {v} 题", "gray"
    if label == "单题止损执行":
        return f"连续 {s['streak']} 场未执行（止损线 {STOP_LOSS_LINE_MINUTES}′）", "red"
    if label == "部分分题数":
        return f"连续 {s['streak']} 场都有 {v} 道半对题", "red"
    if label == "中档题闭环数":
        return f"连续 {s['streak']} 场中档题闭环 {v} 道", "red" if v == 0 else "gray"
    return f"连续 {s['streak']} 场未变", "gray"


def progress_items(contests):
    """跨场「真进步」项（只看最近两场的对比）。"""
    if len(contests) < 2:
        return []
    a, b = contests[-2], contests[-1]

    def signin_min(c):
        return next((p["time_spent_minutes"] for p in c["problems"] if p["tier"] == "签到"), None)

    def delta(cur, prev, invert=False):
        if cur is None or prev is None:
            return None
        d = round(cur - prev, 1)
        return {"from": prev, "to": cur, "delta": d, "good": (d < 0) if invert else (d > 0)}

    items = [
        {"label": "得分率", "unit": "pp",
         "detail": delta(round((b["score_rate"] or 0) * 100, 1), round((a["score_rate"] or 0) * 100, 1))},
        {"label": "完全通过题数", "unit": "题",
         "detail": delta(b["solved_full_count"], a["solved_full_count"])},
        {"label": "中档题闭环数", "unit": "题",
         "detail": delta(b["midtier_closed"], a["midtier_closed"])},
        {"label": "签到题用时", "unit": "分钟",
         "detail": delta(signin_min(b), signin_min(a), invert=True)},
        {"label": "尾段空转", "unit": "分钟",
         "detail": delta(b["idle_tail_minutes"], a["idle_tail_minutes"], invert=True)},
        {"label": "期望分失分（可达）", "unit": "分",
         "detail": delta(b["expected_loss_reachable"], a["expected_loss_reachable"], invert=True)},
    ]
    return [i for i in items if i["detail"] is not None]


# --------------------------------------------------------------------------- #
# 快照（机器可读）与摘要（给 AI 读）
# --------------------------------------------------------------------------- #


def build_snapshot(contests, streaks, training=None, warnings=None):
    """产出稳定的机器可读快照。字段一旦发布尽量只增不改。"""
    n = len(contests)
    last = contests[-1]

    def contest_block(c):
        return {
            "id": c["contest"]["id"],
            "platform": c["contest"]["platform"],
            "name": c["contest"]["name"],
            "date": c["contest"]["date"],
            "contest_system": c["contest"]["contest_system"],
            "is_first_participation": c["contest"].get("is_first_participation"),
            "total_possible": c["scoring"].get("total_possible"),
            "my_total": c["scoring"].get("my_total"),
            "rank": c["scoring"].get("rank"),
            "participants": c["scoring"].get("participants"),
            "percentile": round(c["percentile"], 4) if c["percentile"] is not None else None,
            "score_rate": round(c["score_rate"], 4) if c["score_rate"] is not None else None,
            "solved_full_count": c["solved_full_count"],
            "partial_count": c["partial_count"],
            "problem_count": len(c["problems"]),
            "midtier": {"closed": c["midtier_closed"], "total": c["midtier_total"]},
            "by_tier": {t: dict(v) for t, v in c["tiers"].items() if v["total"]},
            "efficiency": {
                "signin_minutes": next((p["time_spent_minutes"] for p in c["problems"] if p["tier"] == "签到"), None),
                "total_minutes_used": c["total_minutes_used"],
                "idle_tail_minutes": c["idle_tail_minutes"],
                "score_per_minute": round(c["score_per_minute"], 3) if c["score_per_minute"] else None,
            },
            "discipline": {
                "stop_loss_executed": c["stop_loss_executed"],
                "stop_loss_violations": c["stop_loss_violations"],
                "ordering_followed": c["ordering_followed"],
                "expected_ordering": c["ordering_want"],
                "attempted_nontrivial": c["ordering_got"],
            },
            "gaps": {
                "expected_loss_reachable": c["expected_loss_reachable"],
                "missing_highest_expected": (
                    {"letter": c["missing_highest_expected"]["letter"],
                     "expected_score": c["missing_highest_expected"]["expected_score"]}
                    if c["missing_highest_expected"] else None
                ),
            },
            "tier_border_problems": [p["letter"] for p in c["problems"] if p["tier_border"]],
            "problems": [
                {
                    "letter": p["letter"], "title": p["title"], "full_score": p["full_score"],
                    "tier": p["tier"], "tier_border": p["tier_border"],
                    "primary_rate": round(p["_primary_rate"], 4),
                    "expected_score": p["expected_score"], "my_score": p["my_score"],
                    "status": p["status"], "attempted": p["attempted"],
                    "time_spent_minutes": p["time_spent_minutes"],
                    "post_contest_score": p.get("post_contest_score"),
                    "tags": p.get("tags", []), "notes": p.get("notes"),
                }
                for p in c["problems"]
            ],
            "timeline": c.get("timeline", []),
            "timeline_reconstructed": c.get("timeline_reconstructed", False),
            "provenance": c.get("provenance", {}),
        }

    stalled = [s for s in streaks if s["stalled"]]
    snap = {
        "schema_version": SCHEMA_VERSION,
        "generated_at": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        "source": "真实比赛/比赛记录/*.json + 训练记录/日志/*.json",
        "sources": {
            "contests": "真实比赛/比赛记录/*.json",
            "training": "训练记录/日志/*.json",
        },
        "generator": "可视化/growth_core.py",
        "contests_count": n,
        "latest_contest_id": last["contest"]["id"],
        "config": {
            "stop_loss_line_minutes": STOP_LOSS_LINE_MINUTES,
            "reachable_rate_floor": REACHABLE_RATE_FLOOR,
            "tier_bounds": [{"tier": t, "min_rate": lo} for t, lo in TIER_BOUNDS],
            "trend_min_contests": TREND_MIN_CONTESTS,
            "regression_min_contests": REG_COEF_MIN_CONTESTS,
        },
        "latest": contest_block(last),
        "stagnation": [
            {**s, "verdict": stagnation_verdict(s)[0], "severity": stagnation_verdict(s)[1],
             "value_display": ("是" if s["value"] else "否") if isinstance(s["value"], bool) else s["value"]}
            for s in stalled
        ],
        "progress": progress_items(contests),
        "training": training if training is not None else {"status": "empty"},
        "constraints": [
            f"比赛场数 {n} < {TREND_MIN_CONTESTS}：禁止画跨场趋势线，只能用对比表" if n < TREND_MIN_CONTESTS else "可画比赛跨场趋势线",
            f"比赛场数 {n} < {REG_COEF_MIN_CONTESTS}：禁止回归/预测" if n < REG_COEF_MIN_CONTESTS else "可做回归",
            f"比赛场数 {n} < {RADAR_MIN_CONTESTS}：禁止雷达面积对比" if n < RADAR_MIN_CONTESTS else "可做雷达面积对比",
        ] + (training.get("constraints", []) if training else []),
        "warnings": warnings or [],
        "history": [contest_block(c) for c in contests],
    }
    return snap


def build_brief(snap):
    """给 AI 读的精简 Markdown 摘要（约 1–2 KB），供直接贴进上下文。"""
    L = []
    L.append(f"# 算法成长摘要（{snap['generated_at']} · 共 {snap['contests_count']} 场）")
    L.append("")
    L.append(f"真值源：`{snap['source']}` ｜ 全量数据：`成长数据/snapshot.json`")
    L.append("")

    # 停滞（最重要，放最前）
    L.append("## ⚠️ 停滞（连续多场未变，优先处理）")
    if snap["stagnation"]:
        for s in snap["stagnation"]:
            L.append(f"- **{s['label']}**：{s['verdict']}")
    else:
        L.append("- 无")
    L.append("")

    # 进步
    L.append("## 真进步（最近两场）")
    good = [p for p in snap["progress"] if p["detail"]["good"]]
    if good:
        for p in good:
            d = p["detail"]
            L.append(f"- {p['label']}：{d['from']} → {d['to']}（{d['delta']:+g} {p['unit']}）")
    else:
        L.append("- 无")
    L.append("")

    # 最新一场
    c = snap["latest"]
    L.append(f"## 最近一场 · {c['name']}（{c['date']}）")
    pct = f"{c['percentile'] * 100:.1f}%" if c["percentile"] is not None else "数据缺失"
    L.append(f"- 得分 {c['my_total']:g}/{c['total_possible']:g}"
             f"（{c['score_rate'] * 100:.1f}%），名次 {c['rank'] if c['rank'] is not None else '—'}"
             f"（百分位 {pct}）")
    L.append(f"- 完全通过 {c['solved_full_count']}/{c['problem_count']}，部分分 {c['partial_count']}"
             f"，中档闭环 {c['midtier']['closed']}/{c['midtier']['total']}")
    L.append(f"- 止损 {'已执行' if c['discipline']['stop_loss_executed'] else '未执行'}"
             f"｜尾段空转 {c['efficiency']['idle_tail_minutes']:g}′"
             f"｜得分速度 {c['efficiency']['score_per_minute'] if c['efficiency']['score_per_minute'] else '—'} 分/分")
    gaps = c["gaps"]
    if gaps["missing_highest_expected"]:
        m = gaps["missing_highest_expected"]
        L.append(f"- 期望分最高的零分题：**{m['letter']}**（期望 {m['expected_score']:g}）"
                 f"｜本场期望分失分(可达) {gaps['expected_loss_reachable']:g}")
    if c["tier_border_problems"]:
        L.append(f"- ⚠️ 档位边界题：{'/'.join(c['tier_border_problems'])}（通过率距边界 ≤2pp，判定存疑）")
    L.append("")

    # 训练近况（第二个真值源）
    t = snap.get("training") or {}
    L.append("## 训练近况（日常训练真值）")
    if not t or t.get("status") == "empty":
        L.append("- 训练记录尚未录入。真值源：`训练记录/日志/`（一天一个文件，逐题记）")
    else:
        L.append(f"- 训练日 {t['days']} 天（{t['date_first']} ~ {t['date_last']}）"
                 f"｜累计 {t['problems_total']} 题｜AC {t['ac_count']}"
                 f"｜日均 {t['problems_per_day']} 题")
        if t["independent_ac_rate"] is None:
            L.append(f"- **独立 AC 率：不可计算** — 有完成度标注的题数 = 0"
                     f"（{t['weak_problem_count']} 道为弱证据，只能证明「做过」）")
        else:
            L.append(f"- **独立 AC 率：{t['independent_ac_rate'] * 100:.1f}%**"
                     f"（基于 {t['scored_count']} 道有完成度标注的题）"
                     f"｜依赖题解率 {t['dependency_rate'] * 100:.1f}%")
            L.append(f"- 构成：独立 {t['independent_count']} / 卡住后自己过 {t['hinted_count']}"
                     f" / 看题解后过 {t['solution_dependent_count']}")
        ep = t["evidence_problems"]
        L.append(f"- 证据分级：强 {ep.get('强', 0)} / 中 {ep.get('中', 0)} / 弱 {ep.get('弱', 0)}"
                 f"（弱证据题 **不参与** 独立 AC 率）")
        if t["by_tag"]:
            top = list(t["by_tag"].items())[:6]
            seg = "、".join(
                f"{k} {v['independent']}/{v['scored']}"
                + (f"（{v['independent_rate'] * 100:.0f}%）" if v["independent_rate"] is not None else "")
                for k, v in top)
            L.append(f"- 按标签独立 AC：{seg}")
        if t["median_minutes_per_problem"] is not None:
            L.append(f"- 单题中位耗时：{t['median_minutes_per_problem']:g} 分钟")
    L.append("")

    # 档位分布
    L.append("## 难度档（累计，按封榜通过率）")
    agg = OrderedDict()
    for h in snap["history"]:
        for t, v in h["by_tier"].items():
            a = agg.setdefault(t, {"total": 0, "full": 0})
            a["total"] += v["total"]
            a["full"] += v["full"]
    for t, v in agg.items():
        L.append(f"- {t}：完全通过 {v['full']}/{v['total']}")
    L.append("")

    # 约束与警告
    if snap["constraints"] or snap["warnings"]:
        L.append("## 约束 / 警告")
        for s in snap["constraints"]:
            L.append(f"- {s}")
        for w in snap["warnings"]:
            L.append(f"- ⚠️ {w}")
        L.append("")

    L.append("> AI 提示：以本摘要为准做分析；需要逐题/时间线明细时读 `成长数据/snapshot.json`。"
             "数据变动后重跑 `python 可视化/build.py`。")
    L.append("> 本人只写根目录 `随手记.md`（自由格式），结构性数据由 AI 整理——不要让本人去填 JSON。")
    return "\n".join(L)


def export(records_dir=RECORDS_DIR, training_dir=TRAINING_DIR, data_dir=DATA_DIR):
    """读取两个真值源 → 写出 snapshot.json 与 brief.md。返回 (snap, 路径信息)。"""
    warnings, errors = check_all(records_dir)
    if errors:
        raise GrowthError("数据校验失败：\n  - " + "\n  - ".join(errors))
    contests = enrich_all(load_contests(records_dir))
    streaks = compute_streaks(contests)
    training = build_training_block(load_training(training_dir))
    snap = build_snapshot(contests, streaks, training=training, warnings=warnings)
    data_dir.mkdir(parents=True, exist_ok=True)
    snap_path = data_dir / "snapshot.json"
    brief_path = data_dir / "brief.md"
    snap_path.write_text(json.dumps(snap, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    brief_path.write_text(build_brief(snap) + "\n", encoding="utf-8")
    return snap, {"snapshot": snap_path, "brief": brief_path}
