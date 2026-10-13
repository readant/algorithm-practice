#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""build.py — 把比赛记录与训练记录汇总成一份摘要（brief.md）和一个网页（dashboard.html）。

用法（给 AI 用，人不用跑）：
    python 可视化/build.py            # 汇总 + 更新两份输出
    python 可视化/build.py --open     # 顺便打开网页

只读 真实比赛/比赛记录/ 与 训练记录/日志/，只写 成长数据/ 与 可视化/dashboard.html。

诚实约束（别绕过）
------------------
1. 场数 < 3 时**不做**「连续 / 趋势 / 进步」这类表述，只列事实。
2. 没有「是否独立」标注的题**不计入**独立 AC 率。
3. 记不清的字段写 null，不要估。
"""

import argparse
import json
import sys
import webbrowser
from datetime import datetime
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CONTESTS = ROOT / "真实比赛" / "比赛记录"
TRAINING = ROOT / "训练记录" / "日志"
OUT = ROOT / "成长数据"
PAGE = ROOT / "可视化" / "记录.html"

STOP_LOSS = 25             # 单题止损线（分钟）
REACHABLE = 0.30           # 期望分失分只统计封榜通过率 >= 30% 的题
MIN_N = 3                  # 少于此值就明确提示「数据不足以判断趋势」（本脚本本身不做趋势判断）
TIERS = [("签到", 0.90), ("简单", 0.60), ("中档", 0.30), ("困难", 0.0)]
INDEP = ("独立", "卡住后自己过", "看题解后过")


class Err(Exception):
    pass


# --------------------------------------------------------------------------- #
# 读取
# --------------------------------------------------------------------------- #


def _load(d, name):
    files = sorted(p for p in d.glob("*.json") if not p.name.startswith("_"))
    if not d.is_dir() or not files:
        return []
    out = []
    for p in files:
        try:
            out.append(json.loads(p.read_text(encoding="utf-8")))
        except json.JSONDecodeError as e:
            raise Err(f"{name}/{p.name} 不是合法 JSON：{e}")
    return out


def _rate(node):
    """优先用 分子/分母 现算，避免手写 rate 的舍入误差。"""
    if not node:
        return None
    n, d = node.get("numerator"), node.get("denominator")
    return n / d if (n is not None and d) else node.get("rate")


def _tier(rate):
    if rate is None:
        return None
    for name, lo in TIERS:
        if rate >= lo:
            return name
    return "困难"


# --------------------------------------------------------------------------- #
# 计算：比赛
# --------------------------------------------------------------------------- #


def contest_facts(c):
    """把一场比赛算成一组纯事实（不下判断）。"""
    probs = []
    for p in c["problems"]:
        key = {"封榜": "frozen", "官方终版": "official_final", "官方当场": "official_insession"}[
            p["pass_rates"]["primary_source"]]
        rate = _rate(p["pass_rates"][key])
        if rate is None:
            raise Err(f"题 {p['letter']} 的 primary_source 指向的通过率为 null")
        probs.append({
            "letter": p["letter"], "title": p.get("title") or "",
            "full": p["full_score"], "rate": rate, "tier": _tier(_tier_rate(p)),
            "expect": round(rate * p["full_score"], 1), "my": p["my_score"],
            "status": p["status"], "minutes": p.get("time_spent_minutes"),
            "note": p.get("notes"),
        })
    sc = c["scoring"]
    total, mine = sc.get("total_possible"), sc.get("my_total")
    violations = [p["letter"] for p in probs
                  if p["minutes"] and p["minutes"] > STOP_LOSS and p["status"] != "full"]
    zero = [p for p in probs if p["my"] == 0 and p["expect"] > 0]
    return {
        "date": c["contest"]["date"], "name": c["contest"]["name"],
        "total": total, "my": mine,
        "rate": (mine / total) if (total and mine is not None) else None,
        "rank": sc.get("rank"), "players": sc.get("participants"),
        "full_count": sum(1 for p in probs if p["status"] == "full"),
        "partial": sum(1 for p in probs if p["status"] == "partial"),
        "n_problems": len(probs),
        "idle": round(sum(s["minutes"] for s in c.get("timeline", []) if s.get("kind") == "空转"), 1),
        "stop_loss_violations": violations,
        "worst_zero": max(zero, key=lambda x: x["expect"]) if zero else None,
        "lost_reachable": round(sum(max(0, p["expect"] - p["my"]) for p in probs if p["rate"] >= REACHABLE), 1),
        "problems": probs,
        "sources": c.get("provenance", {}).get("sources", []),
    }


def _tier_rate(p):
    pr = p["pass_rates"]
    for k in ("frozen", "official_insession", "official_final"):
        r = _rate(pr.get(k))
        if r is not None:
            return r
    return None


# --------------------------------------------------------------------------- #
# 计算：训练
# --------------------------------------------------------------------------- #


def training_facts(days):
    if not days:
        return None
    rows = [(d, p) for d in days for p in d["problems"]]
    marked = [(d, p) for d, p in rows if p.get("independent") in INDEP]
    n_ind = sum(1 for _, p in marked if p["independent"] == "独立")
    n_hint = sum(1 for _, p in marked if p["independent"] == "卡住后自己过")
    n_dep = sum(1 for _, p in marked if p["independent"] == "看题解后过")
    ev = {}
    for d in days:
        ev[d["evidence_level"]] = ev.get(d["evidence_level"], 0) + len(d["problems"])
    return {
        "days": len({d["date"] for d in days}),
        "first": min(d["date"] for d in days), "last": max(d["date"] for d in days),
        "total": len(rows), "ac": sum(1 for _, p in rows if p["verdict"] == "AC"),
        "marked": len(marked), "ind": n_ind, "hint": n_hint, "dep": n_dep,
        "ind_rate": round(n_ind / len(marked), 4) if marked else None,
        "ev": {"强": ev.get("强", 0), "中": ev.get("中", 0), "弱": ev.get("弱", 0)},
        "days_detail": days,
    }


# --------------------------------------------------------------------------- #
# 输出 1：给 AI 读的摘要
# --------------------------------------------------------------------------- #


def build_brief(cs, tr):
    L = [f"# 算法练习摘要（{datetime.now():%Y-%m-%d %H:%M} · 比赛 {len(cs)} 场"
         + (f" · 训练 {tr['days']} 天" if tr else "") + "）", ""]
    L.append("数据来自 `真实比赛/比赛记录/` 与 `训练记录/日志/`；逐题明细见 `成长数据/snapshot.json`。")
    L.append("")

    L.append("## 比赛（逐场事实）")
    if not cs:
        L.append("- 暂无记录。")
    for c in cs:
        rk = (f"名次 {c['rank']}/{c['players']}" if c["players"]
              else (f"名次 {c['rank']}（总人数未知）" if c["rank"] else "名次 —"))
        rank_pct = f"{c['rank'] / c['players'] * 100:.1f}%" if (c["rank"] and c["players"]) else None
        L.append(f"- **{c['date']} {c['name']}**：{c['my']:g}/{c['total']:g}（{c['rate'] * 100:.1f}%）"
                 f"，{rk}" + (f"（百分位 {rank_pct}）" if rank_pct else "")
                 + f"，完全通过 {c['full_count']}/{c['n_problems']}，部分分 {c['partial']}")
        L.append(f"  - 尾段空转 {c['idle']:g}′；单题止损（{STOP_LOSS}′线）"
                 + ("未见违规" if not c["stop_loss_violations"] else f"违规题：{'/'.join(c['stop_loss_violations'])}"))
        if c["worst_zero"]:
            w = c["worst_zero"]
            L.append(f"  - 期望分最高的零分题：{w['letter']}（期望 {w['expect']:g}）；"
                     f"本场期望分失分（可达）{c['lost_reachable']:g}")
    if cs and len(cs) < MIN_N:
        L.append("")
        L.append(f"> 只有 {len(cs)} 场，**不足以判断趋势或「进步」**——这里只列事实，不下结论。")
    L.append("")

    L.append("## 训练")
    if not tr:
        L.append("- 还没有训练记录。在 `随手记.md` 里一行一题写下，让 AI 整理。")
    else:
        L.append(f"- 训练 {tr['days']} 天（{tr['first']} ~ {tr['last']}），累计 {tr['total']} 题，AC {tr['ac']}")
        if tr["ind_rate"] is None:
            L.append(f"- **独立完成率：不可计算** —— {tr['total']} 道题都没有「是否独立」的标注。")
            L.append("  这批数据只能说明「做过」，说明不了「自己做出来的」。")
        else:
            L.append(f"- **独立完成率：{tr['ind_rate'] * 100:.1f}%**"
                     f"（基于 {tr['marked']} 道有标注的题；另有 {tr['total'] - tr['marked']} 道无标注）")
            L.append(f"- 构成：独立 {tr['ind']} / 卡住后自己过 {tr['hint']} / 看题解后过 {tr['dep']}")
        L.append(f"- 数据可信度：强 {tr['ev']['强']} / 中 {tr['ev']['中']} / 弱 {tr['ev']['弱']} 题"
                 f"（弱＝只有题号、没代码没截图）")
    L.append("")
    L.append("> 给 AI：本摘要只列事实。要下「进步/退步」这类结论前，先看场数与训练天数是否够；不够就直说不够。")
    return "\n".join(L)


# --------------------------------------------------------------------------- #
# 输出 2：网页（事实报告，不是分析仪表盘）
# --------------------------------------------------------------------------- #

CSS = """
body{margin:0;background:#fff;color:#1f2328;font:14px/1.7 -apple-system,"Segoe UI","Microsoft YaHei",sans-serif}
.wrap{max-width:880px;margin:0 auto;padding:32px 24px 64px}
h1{font-size:20px;margin:0 0 4px} h2{font-size:15px;margin:30px 0 8px;padding-bottom:5px;border-bottom:1px solid #d8dee4}
.sub{color:#6b7280;font-size:12.5px;margin:0 0 12px} .note{color:#92400e;background:#fffbeb;border-left:3px solid #d97706;
padding:9px 12px;border-radius:4px;font-size:12.5px;margin:10px 0}
table{width:100%;border-collapse:collapse;font-size:13px} th,td{padding:7px 9px;border-bottom:1px solid #e5e7eb;text-align:left}
th{color:#6b7280;font-weight:500;background:#f6f7f9} td.n{font-variant-numeric:tabular-nums}
.mut{color:#9ca3af} .bad{color:#dc2626} code{background:#eef1f4;padding:1px 5px;border-radius:3px;font-size:12px}
.bar{display:flex;height:22px;border-radius:4px;overflow:hidden;border:1px solid #d8dee4;margin-top:4px}
.bar i{display:flex;align-items:center;justify-content:center;color:#fff;font-size:11px;font-style:normal}
footer{margin-top:36px;padding-top:12px;border-top:1px solid #e5e7eb;color:#6b7280;font-size:12px}
"""


def esc(s):
    return str(s).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def build_page(cs, tr):
    n = len(cs)
    P = ['<!DOCTYPE html><html lang="zh-CN"><head><meta charset="utf-8">',
         '<meta name="viewport" content="width=device-width,initial-scale=1">',
         "<title>算法练习记录</title>", f"<style>{CSS}</style></head><body><div class=wrap>",
         "<h1>算法练习记录</h1>",
         f'<p class=sub>比赛 {n} 场' + (f" · 训练 {tr['days']} 天" if tr else "")
         + " · 数据来自 <code>真实比赛/</code> 与 <code>训练记录/</code> · 由 <code>可视化/build.py</code> 生成</p>"]

    if not cs and not tr:
        P.append('<div class=note>还没有任何记录。在根目录 <code>随手记.md</code> 里写一行一题，让 AI 整理。</div>')

    # 逐场
    if cs:
        P.append("<h2>比赛</h2>")
        P.append("<table><tr><th>日期</th><th>赛事</th><th>得分/满分</th><th>得分率</th>"
                 "<th>名次</th><th>完全通过</th><th>部分分</th><th>尾段空转</th><th>止损违规</th></tr>")
        for c in cs:
            rk = f"{c['rank']}/{c['players']}" if c["players"] else (c["rank"] or "—")
            P.append(f"<tr><td>{c['date']}</td><td>{esc(c['name'])}</td>"
                     f"<td class=n>{c['my']:g}/{c['total']:g}</td><td class=n>{c['rate'] * 100:.1f}%</td>"
                     f"<td class=n>{rk}</td><td class=n>{c['full_count']}/{c['n_problems']}</td>"
                     f"<td class=n>{c['partial']}</td><td class=n>{c['idle']:g}′</td>"
                     f"<td>{'—' if not c['stop_loss_violations'] else '<span class=bad>' + '/'.join(c['stop_loss_violations']) + '</span>'}</td></tr>")
        P.append("</table>")
        if n < MIN_N:
            P.append(f'<div class=note>只有 {n} 场，不足以判断趋势或"进步"。本页只列事实。</div>')
        for c in cs:
            P.append(f"<h2>{esc(c['name'])} · 逐题</h2>")
            P.append("<table><tr><th>题</th><th>满分</th><th>通过率</th><th>难度档</th>"
                     "<th>期望分</th><th>我的得分</th><th>用时</th><th>状态</th></tr>")
            for p in c["problems"]:
                st = {"full": "✅", "partial": "⚠️ 部分分", "failed": "❌ 未过", "unattempted": "未做"}[p["status"]]
                mins = f"{p['minutes']:g}′" if p["minutes"] else "—"
                P.append(f"<tr><td>{p['letter']} · {esc(p['title'])}</td><td class=n>{p['full']:g}</td>"
                         f"<td class=n>{p['rate'] * 100:.1f}%</td><td>{p['tier'] or '—'}</td>"
                         f"<td class=n>{p['expect']:g}</td><td class=n>{p['my']:g}</td>"
                         f"<td class=n>{mins}</td><td>{st}</td></tr>")
            P.append("</table>")

    # 训练
    P.append("<h2>训练</h2>")
    if not tr:
        P.append('<p class=sub>还没有训练记录。<code>随手记.md</code> 里一行一题写下即可。</p>')
    else:
        rate = f"{tr['ind_rate'] * 100:.1f}%" if tr["ind_rate"] is not None else "不可计算"
        P.append("<table><tr><th>训练日</th><th>题数</th><th>AC</th><th>有独立标注</th>"
                 f"<th>独立完成率</th><th>数据可信度（强/中/弱）</th></tr>")
        P.append(f"<tr><td class=n>{tr['days']}</td><td class=n>{tr['total']}</td><td class=n>{tr['ac']}</td>"
                 f"<td class=n>{tr['marked']}</td><td class=n>{rate}</td>"
                 f"<td class=n>{tr['ev']['强']} / {tr['ev']['中']} / {tr['ev']['弱']}</td></tr></table>")
        parts = [("独立", tr["ind"], "#16a34a"), ("卡住后自己过", tr["hint"], "#d97706"),
                 ("看题解后过", tr["dep"], "#dc2626"), ("未标注", tr["total"] - tr["marked"], "#9ca3af")]
        tot = sum(x[1] for x in parts) or 1
        P.append('<div class=bar>' + "".join(
            f'<i style="width:{v / tot * 100:.1f}%;background:{c}">{k[:4]} {v}</i>'
            for k, v, c in parts if v) + "</div>")
        if tr["ind_rate"] is None:
            P.append('<div class=note>没有一道题标了「是否独立」，所以独立完成率算不出来。'
                     '这批数据只能说明"做过"。下次在随手记里多写一个词就行。</div>')

    P.append("<h2>数据说明</h2>")
    P.append("<table><tr><th>来源</th><th>内容</th><th>说明</th></tr>"
             "<tr><td><code>真实比赛/</code></td><td>赛场成绩、赛内代码</td><td>外部可核验</td></tr>"
             "<tr><td><code>训练记录/</code></td><td>日常训练、是否独立</td><td>自述，分强/中/弱</td></tr>"
             "<tr><td><code>学习模块/</code>、<code>题解归档/</code></td><td>讲义、题解</td>"
             "<td>学习材料，<b>不代表水平</b></td></tr></table>")
    P.append(f"<footer>生成时间 {datetime.now():%Y-%m-%d %H:%M} · 止损线 {STOP_LOSS}′ · "
             f"场数 &lt; {MIN_N} 不做趋势判断</footer></div></body></html>")
    return "".join(P)


# --------------------------------------------------------------------------- #
# 入口
# --------------------------------------------------------------------------- #


def main():
    ap = argparse.ArgumentParser(description="汇总比赛与训练记录")
    ap.add_argument("--open", action="store_true", help="生成后打开网页")
    a = ap.parse_args()
    try:
        cs = [contest_facts(c) for c in _load(CONTESTS, "真实比赛")]
        cs.sort(key=lambda x: x["date"])
        tr = training_facts(_load(TRAINING, "训练记录"))
    except Err as e:
        print(f"[错误] {e}", file=sys.stderr)
        return 1

    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / "brief.md").write_text(build_brief(cs, tr) + "\n", encoding="utf-8")
    (OUT / "snapshot.json").write_text(
        json.dumps({"generated": datetime.now().isoformat(timespec="seconds"),
                    "contests": cs, "training": tr}, ensure_ascii=False, indent=1) + "\n",
        encoding="utf-8")
    PAGE.write_text(build_page(cs, tr), encoding="utf-8")

    print(f"[完成] 比赛 {len(cs)} 场" + (f" · 训练 {tr['days']} 天" if tr else " · 训练层为空"))
    print(f"       {OUT.relative_to(ROOT)}/brief.md · snapshot.json · {PAGE.relative_to(ROOT)}")
    if a.open:
        webbrowser.open(PAGE.as_uri())
    return 0


if __name__ == "__main__":
    sys.exit(main())
