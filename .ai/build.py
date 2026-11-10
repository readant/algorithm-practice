#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""build.py — 把比赛记录与训练记录汇总成一份摘要（brief.md）和一个网页（记录.html）。

用法（给 AI 用，人不用跑）：
    python .ai/build.py            # 汇总 + 更新两份输出
    python .ai/build.py --open     # 顺便打开网页

只读 .ai/真实比赛/比赛记录/ 与 .ai/训练记录/日志/，
写 .ai/输出/{brief.md,snapshot.json} 与 仓库根的「记录.html」（那个是给人看的，所以放外面）。

所有图都在这一页里：汇总表的得分率条、尾段空转对比、每场的「时间去向」图。
时间去向图由 timeline 现算成内联 SVG，不依赖前端库，断网也能打开——
**不要为单场比赛另出一个 html**（2026-10-19 L君 明确要求）。

诚实约束（别绕过）
------------------
1. 场数 < 3 时**不做**「连续 / 趋势 / 进步」这类表述，只列事实。
2. 没有「是否独立」标注的题**不计入**独立完成率。
3. 记不清的字段写 null，不要估。
"""

import argparse
import json
import math
import sys
import webbrowser
from datetime import datetime
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent      # 仓库根
AI = ROOT / ".ai"                                  # 机器层
CONTESTS = AI / "真实比赛" / "比赛记录"
TRAINING = AI / "训练记录" / "日志"
OUT = AI / "输出"
PAGE = ROOT / "记录.html"                          # 给人看的，放外层
ABILITY = ROOT / "能力概览.html"                   # 跨场能力画像，给人看的

STOP_LOSS = 25             # 单题止损线（分钟）
REACHABLE = 0.30           # 期望分失分只统计封榜通过率 >= 30% 的题
MIN_N = 3                  # 少于此值就明确提示「数据不足以判断趋势」（本脚本本身不做趋势判断）
TIERS = [("签到", 0.90), ("简单", 0.60), ("中档", 0.30), ("困难", 0.0)]
INDEP = ("独立", "卡住后自己过", "看题解后过")
PLAYER = "读书蚁"          # 能力概览页显示的名字，改这里即可

# 六维：key / 名称 / 算法。顺序即雷达图从顶部顺时针的顺序。
# 全部是「比率」，不给分、不加权；某一维没有数据就留空，绝不补 0。
DIMS = [
    ("rate",   "得分率",     "总分 ÷ 总满分"),
    ("full",   "完全通过率",  "满分题数 ÷ 总题数"),
    ("mid",    "中档题得分",  "通过率 30–60% 的题上：我的得分 ÷ 该题满分"),
    ("ontime", "时间纪律",   "未超 25′ 止损线的题数 ÷ 有计时的题数"),
    ("over",   "相对期望",   "我的总分 ÷ 全场期望分（100% = 正好达到全场平均）"),
    ("indep",  "独立完成率",  "标了「是否独立」的题里独立做出 ÷ 标注总数"),
]

# 知识点归类：把题目里的人工 tag 归成算法大类。一道题可落到多个类（例如「模拟 + 前缀和」）。
# 只收算法知识点；「复杂度估算 / 输出格式 / 数据规模放大 / Special Judge」属工程性失误，不进这张表。
TOPICS = [
    ("模拟与基础", {"签到", "模拟", "输入输出", "数组", "遍历", "分类讨论", "极差",
                    "读入后取 max-min", "抹零", "整数运算", "奇偶下标", "奇偶判断", "奇偶性", "分组"}),
    ("构造与贪心", {"构造", "贪心", "排列", "逆向", "枚举", "暴力", "枚举上界", "剪枝", "枚举根"}),
    ("前缀和与差分", {"前缀和", "差分", "差分/前缀和", "区间和", "前缀交替和"}),
    ("排序与二分", {"排序", "二分查找", "二分答案", "树状数组上二分", "单调性", "离散化", "扫描线"}),
    ("字符串", {"字符串", "后缀判断", "游程压缩", "字符计数", "集合交集"}),
    ("搜索与网格", {"BFS", "二维网格", "边界判定", "网格遍历", "闭环", "方向判定", "有向面积"}),
    ("动态规划", {"DP", "树形DP", "换根", "概率期望"}),
    ("数与计数", {"数论", "gcd", "整除", "同余", "负数取模", "取模", "取模逆元",
                  "乘法原理", "计数", "3-smooth", "倍数枚举", "约数奇偶", "素数因子"}),
    ("图与树", {"树", "并查集", "祖先关系", "树上计数", "基环树", "树上距离", "模3计数",
                "环形数组", "二分图匹配"}),
    ("博弈", {"博弈"}),
]


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
            "note": p.get("notes"), "tags": p.get("tags") or [],
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
        "rating": sc.get("rating"), "rating_delta": sc.get("rating_delta"),
        "full_count": sum(1 for p in probs if p["status"] == "full"),
        "partial": sum(1 for p in probs if p["status"] == "partial"),
        "n_problems": len(probs),
        "idle": round(sum(s["minutes"] for s in c.get("timeline", []) if s.get("kind") == "空转"), 1),
        "stop_loss_violations": violations,
        "worst_zero": max(zero, key=lambda x: x["expect"]) if zero else None,
        "lost_reachable": round(sum(max(0, p["expect"] - p["my"]) for p in probs if p["rate"] >= REACHABLE), 1),
        "problems": probs,
        "sources": c.get("provenance", {}).get("sources", []),
        "pass_source": c["problems"][0]["pass_rates"]["primary_source"] if c["problems"] else None,
        "timeline": c.get("timeline", []),
        "timeline_reconstructed": bool(c.get("timeline_reconstructed")),
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
    L.append("数据来自 `.ai/真实比赛/比赛记录/` 与 `.ai/训练记录/日志/`；逐题明细见 `.ai/输出/snapshot.json`。")
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
            src_note = "" if c["pass_source"] == "封榜" else f"（口径 {c['pass_source']}，非封榜口径，仅参考）"
            L.append(f"  - 期望分最高的零分题：{w['letter']}（期望 {w['expect']:g}）{src_note}；"
                     f"本场期望分失分（可达）{c['lost_reachable']:g}")
    if cs and len(cs) < MIN_N:
        L.append("")
        L.append(f"> 只有 {len(cs)} 场，**不足以判断趋势或「进步」**——这里只列事实，不下结论。")
    L.append("")

    L.append("## 训练")
    if not tr:
        L.append("- 还没有训练记录。在 `随手记/` 里一行一题写下，让 AI 整理。")
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
.mini{height:3px;background:#eef1f4;border-radius:2px;margin-top:5px;overflow:hidden;min-width:52px}
.mini>i{display:block;height:100%;border-radius:2px}
.cmp{max-width:560px;margin:10px 0 2px}
.cmp .r{display:flex;align-items:center;gap:10px;margin:6px 0;font-size:12px}
.cmp .r b{width:64px;font-weight:400;color:#6b7280;font-variant-numeric:tabular-nums}
.cmp .r .t{flex:1;height:14px;background:#f1efe8;border-radius:3px;overflow:hidden}
.cmp .r .t>i{display:block;height:100%;background:#888780}
.cmp .r em{width:48px;text-align:right;font-style:normal;color:#1f2328;font-variant-numeric:tabular-nums}
footer{margin-top:36px;padding-top:12px;border-top:1px solid #e5e7eb;color:#6b7280;font-size:12px}
"""


def esc(s):
    return str(s).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def svg_time(c):
    """把一场的 timeline 画成横向条形（纯 SVG，无外部依赖，离线可用）。"""
    tl = c.get("timeline") or []
    if not tl:
        return ""
    mx = max(t["minutes"] for t in tl) or 1
    x0, w = 160, 396
    h = len(tl) * 40 + 48
    o = [f'<svg viewBox="0 0 680 {h}" width="100%" role="img" xmlns="http://www.w3.org/2000/svg">',
         f'<title>{esc(c["name"])} 的时间去向</title>',
         '<desc>逐题用时，以及与末次提交后的空转分钟数。</desc>',
         '<g font-family="-apple-system, \'Segoe UI\', \'Microsoft YaHei\', sans-serif">',
         f'<text x="{x0}" y="20" font-size="11" fill="#6b7280">分钟</text>']
    notes = []
    for i, t in enumerate(tl):
        y = 30 + i * 40
        m = t["minutes"]
        bw = max(m / mx * w, 2)
        col = ("#888780" if t.get("kind") == "空转"
               else "#0F6E56" if t.get("score_gained") else "#BA7517")
        o.append(f'<text x="152" y="{y + 11}" font-size="12.5" fill="#1f2328" '
                 f'text-anchor="end" dominant-baseline="central">{esc(t["stage"])}</text>')
        o.append(f'<rect x="{x0}" y="{y}" width="{bw:.1f}" height="22" rx="3" fill="{col}"/>')
        o.append(f'<text x="{x0 + bw + 7:.1f}" y="{y + 11}" font-size="11.5" fill="#1f2328" '
                 f'dominant-baseline="central">{m:g}</text>')
    if c.get("timeline_reconstructed"):
        notes.append("时间线为赛后重构，不是精确记录")
    timed = sum(1 for t in tl if t.get("kind") == "题")
    if timed < len(c.get("problems") or []):
        notes.append("未提交的题未单独计时")
    if notes:
        o.append(f'<text x="{x0}" y="{h - 12}" font-size="11" fill="#9ca3af">'
                 + "；".join(notes) + "</text>")
    o.append("</g></svg>")
    return "".join(o)


def build_page(cs, tr):
    n = len(cs)
    P = ['<!DOCTYPE html><html lang="zh-CN"><head><meta charset="utf-8">',
         '<meta name="viewport" content="width=device-width,initial-scale=1">',
         "<title>算法练习记录</title>", f"<style>{CSS}</style></head><body><div class=wrap>",
         "<h1>算法练习记录</h1>",
         f'<p class=sub>比赛 {n} 场' + (f" · 训练 {tr['days']} 天" if tr else "")
         + ' · 数据来自 <code>.ai/</code>（机器层）· 由 <code>.ai/build.py</code> 生成'
         + ' · <a href="能力概览.html">看能力概览 →</a></p>']

    if not cs and not tr:
        P.append('<div class=note>还没有任何记录。在根目录 <code>随手记/</code> 里写一行一题，让 AI 整理。</div>')

    # 逐场
    if cs:
        P.append("<h2>比赛</h2>")
        P.append("<table><tr><th>日期</th><th>赛事</th><th>得分/满分</th><th>得分率</th>"
                 "<th>名次</th><th>完全通过</th><th>部分分</th><th>尾段空转</th><th>止损违规</th></tr>")
        for c in cs:
            rk = f"{c['rank']}/{c['players']}" if c["players"] else (c["rank"] or "—")
            rbar = (f'<div class=mini><i style="width:{min(c["rate"], 1) * 100:.1f}%;'
                    'background:#0F6E56"></i></div>')
            P.append(f"<tr><td>{c['date']}</td><td>{esc(c['name'])}</td>"
                     f"<td class=n>{c['my']:g}/{c['total']:g}</td>"
                     f"<td class=n>{c['rate'] * 100:.1f}%{rbar}</td>"
                     f"<td class=n>{rk}</td><td class=n>{c['full_count']}/{c['n_problems']}</td>"
                     f"<td class=n>{c['partial']}</td><td class=n>{c['idle']:g}′</td>"
                     f"<td>{'—' if not c['stop_loss_violations'] else '<span class=bad>' + '/'.join(c['stop_loss_violations']) + '</span>'}</td></tr>")
        P.append("</table>")
        if len(cs) >= 2:
            mx = max(c["idle"] for c in cs) or 1
            P.append('<p class=sub style="margin:16px 0 2px">尾段空转（末次提交到结束）</p>')
            P.append("<div class=cmp>")
            for c in cs:
                P.append(f'<div class=r><b>{c["date"][5:]}</b>'
                         f'<div class=t><i style="width:{c["idle"] / mx * 100:.1f}%"></i></div>'
                         f'<em>{c["idle"]:g}′</em></div>')
            P.append("</div>")
            P.append('<p class=sub style="margin:6px 0 0">只列各场数值；'
                     '场数不足时不做“越来越差”这类趋势结论。</p>')
        if n < MIN_N:
            P.append(f'<div class=note>只有 {n} 场，不足以判断趋势或"进步"。本页只列事实。</div>')
        for c in cs:
            P.append(f"<h2>{esc(c['name'])} · 逐题</h2>")
            if c.get("pass_source") and c["pass_source"] != "封榜":
                P.append(f"<div class=note>通过率口径：{esc(c['pass_source'])}（非封榜口径）。"
                         "带 * 的难度档按该通过率机械换算，会偏「简单」；"
                         "期望分同样仅供参考，不能与封榜场次横向比较。</div>")
            mark = "*" if (c.get("pass_source") and c["pass_source"] != "封榜") else ""
            P.append("<table><tr><th>题</th><th>满分</th><th>通过率</th><th>难度档</th>"
                     "<th>期望分</th><th>我的得分</th><th>用时</th><th>状态</th></tr>")
            for p in c["problems"]:
                st = {"full": "✅", "partial": "⚠️ 部分分", "failed": "❌ 未过", "unattempted": "未做"}[p["status"]]
                mins = f"{p['minutes']:g}′" if p["minutes"] else "—"
                got = p["my"] / p["full"] if p["full"] else 0
                gcol = "#0F6E56" if got >= 0.999 else ("#888780" if got <= 0 else "#BA7517")
                gbar = (f'<div class=mini><i style="width:{got * 100:.0f}%;'
                        f'background:{gcol}"></i></div>')
                P.append(f"<tr><td>{p['letter']} · {esc(p['title'])}</td><td class=n>{p['full']:g}</td>"
                         f"<td class=n>{p['rate'] * 100:.1f}%</td><td>{(p['tier'] or '—') + mark}</td>"
                         f"<td class=n>{p['expect']:g}</td><td class=n>{p['my']:g}{gbar}</td>"
                         f"<td class=n>{mins}</td><td>{st}</td></tr>")
            P.append("</table>")
            s = svg_time(c)
            if s:
                P.append('<p class=sub style="margin:16px 0 2px">时间去向</p>')
                P.append(s)

    # 训练
    P.append("<h2>训练</h2>")
    if not tr:
        P.append('<p class=sub>还没有训练记录。<code>随手记/</code> 里一行一题写下即可。</p>')
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
             "<tr><td><code>.ai/真实比赛/</code></td><td>赛场成绩、赛内代码</td><td>外部可核验</td></tr>"
             "<tr><td><code>.ai/训练记录/</code></td><td>日常训练、是否独立</td><td>自述，分强/中/弱</td></tr>"
             "<tr><td><code>学习模块/</code>、<code>题解归档/</code></td><td>讲义、题解</td>"
             "<td>学习材料，<b>不代表水平</b></td></tr></table>")
    P.append(f"<footer>生成时间 {datetime.now():%Y-%m-%d %H:%M} · 止损线 {STOP_LOSS}′ · "
             f"场数 &lt; {MIN_N} 不做趋势判断</footer></div></body></html>")
    return "".join(P)


# --------------------------------------------------------------------------- #
# 能力概览页
# --------------------------------------------------------------------------- #


def _dim_one(c):
    """单场的五个可算维度（indep 无数据，不在这里）。"""
    probs = c["problems"]
    exp = sum(p["expect"] for p in probs)
    mid = [p for p in probs if 0.30 <= p["rate"] < 0.60]
    mid_full = sum(p["full"] for p in mid)
    timed = [p for p in probs if p["minutes"] is not None]
    ok = [p for p in timed if p["minutes"] <= STOP_LOSS or p["status"] == "full"]
    return {
        "rate": c["rate"],
        "full": c["full_count"] / c["n_problems"] if c["n_problems"] else None,
        "mid": sum(p["my"] for p in mid) / mid_full if mid_full else None,
        "ontime": len(ok) / len(timed) if timed else None,
        "over": c["my"] / exp if exp else None,
    }


def ability_facts(cs):
    """六维画像：全部用「总量比」，不用「各场比率的平均」，避免小场次权重虚高。"""
    tot = sum(c["total"] or 0 for c in cs)
    mine = sum(c["my"] or 0 for c in cs)
    n_all = sum(c["n_problems"] for c in cs)
    full_all = sum(c["full_count"] for c in cs)
    exp_all = mid_my = mid_full = 0.0
    on_ok = on_n = 0
    for c in cs:
        exp_all += sum(p["expect"] for p in c["problems"])
        for p in c["problems"]:
            if 0.30 <= p["rate"] < 0.60:
                mid_my += p["my"]
                mid_full += p["full"]
            if p["minutes"] is not None:
                on_n += 1
                if p["minutes"] <= STOP_LOSS or p["status"] == "full":
                    on_ok += 1
    return {
        "n": len(cs),
        "dims": {
            "rate": mine / tot if tot else None,
            "full": full_all / n_all if n_all else None,
            "mid": mid_my / mid_full if mid_full else None,
            "ontime": on_ok / on_n if on_n else None,
            "over": mine / exp_all if exp_all else None,
            "indep": None,          # 训练层没有任何「是否独立」标注，不算，也不补 0
        },
        "series": [dict(name=c["name"], date=c["date"], my=c["my"], total=c["total"],
                        full_count=c["full_count"], n_problems=c["n_problems"],
                        rank=c["rank"], players=c["players"],
                        rating=c.get("rating"), rating_delta=c.get("rating_delta"),
                        **_dim_one(c)) for c in cs],
    }


def topic_facts(cs):
    """按知识点大类聚合：这一类题的「我的得分 ÷ 该类满分」。一道题可计入多个类。"""
    out = []
    for name, keys in TOPICS:
        my = full = n = 0
        for c in cs:
            for p in c["problems"]:
                if keys & set(p.get("tags") or []):
                    my += p["my"]
                    full += p["full"]
                    n += 1
        if n:
            out.append((name, {"n": n, "my": my, "full": full,
                               "rate": my / full if full else None}))
    return sorted(out, key=lambda kv: (-kv[1]["n"], kv[0]))


def _pct(v, dash="—"):
    return dash if v is None else f"{v * 100:.1f}%"


def _rank_tier(pct):
    """段位 = 最佳排名百分位机械换算，不含主观评价。"""
    if pct is None:
        return "—", "无可比数据"
    x = pct * 100
    for lo, t in ((5, "S"), (12, "A"), (25, "B"), (45, "C")):
        if x <= lo:
            return t, f"最佳前 {x:.1f}%"
    return "D", f"最佳前 {x:.1f}%"


def _radar_svg(dims):
    cx, cy, R = 268, 196, 116
    n = len(DIMS)

    def pt(i, k):
        a = -math.pi / 2 + i * 2 * math.pi / n
        return cx + R * k * math.cos(a), cy + R * k * math.sin(a)

    s = ['<svg viewBox="0 0 540 396" width="540" height="396" role="img" aria-label="六维能力图" '
         'style="max-width:100%;height:auto;display:block;margin:4px auto 0">']
    for k in (0.2, 0.4, 0.6, 0.8, 1.0):
        pts = " ".join(f"{x:.1f},{y:.1f}" for x, y in (pt(i, k) for i in range(n)))
        s.append(f'<polygon points="{pts}" fill="none" stroke="#243040" stroke-width="1"/>')
    for i, (key, name, _) in enumerate(DIMS):
        x, y = pt(i, 1.0)
        v = dims.get(key)
        s.append(f'<line x1="{cx}" y1="{cy}" x2="{x:.1f}" y2="{y:.1f}" stroke="#243040"'
                 + (' stroke-dasharray="3 5"/>' if v is None else "/>"))
        lx, ly = pt(i, 1.30)
        anchor = "middle" if abs(lx - cx) < 14 else ("start" if lx > cx else "end")
        s.append(f'<text x="{lx:.1f}" y="{ly:.1f}" text-anchor="{anchor}" font-size="13" '
                 f'fill="{("#5c6a7d" if v is None else "#aab6c6")}">{name}</text>')
        s.append(f'<text x="{lx:.1f}" y="{ly + 18:.1f}" text-anchor="{anchor}" font-size="15" '
                 f'font-weight="700" fill="{("#5c6a7d" if v is None else "#4ade80")}">'
                 f'{_pct(v, "未解锁")}</text>')
    pts = " ".join(
        f"{x:.1f},{y:.1f}"
        for x, y in (pt(i, min(dims.get(k) or 0, 1.0)) for i, (k, _, _) in enumerate(DIMS)))
    s.append(f'<polygon points="{pts}" fill="rgba(74,222,128,.13)"/>')
    s.append(f'<polygon points="{pts}" fill="none" stroke="rgba(74,222,128,.28)" stroke-width="7"/>')
    s.append(f'<polygon points="{pts}" fill="none" stroke="#4ade80" stroke-width="2"/>')
    for i, (k, _, _) in enumerate(DIMS):
        v = dims.get(k)
        if v:
            x, y = pt(i, min(v, 1.0))
            s.append(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="3.2" fill="#0a0d13" stroke="#4ade80" '
                     f'stroke-width="2"/>')
    s.append(f'<circle cx="{cx}" cy="{cy}" r="2" fill="#4ade80"/>')
    s.append("</svg>")
    return "".join(s)


def build_ability(cs):
    ab = ability_facts(cs) if cs else {"n": 0, "dims": {}, "series": []}
    d = ab["dims"]
    known = [(k, n, v) for k, n, _ in DIMS if (v := d.get(k)) is not None]

    CSS = (
        "*{box-sizing:border-box}"
        "html{background:#06090d}"
        "body{margin:0;padding:24px 18px 56px;color:#dbe3ee;"
        "font:13px/1.6 -apple-system,'Segoe UI','Microsoft YaHei',sans-serif;"
        "-webkit-font-smoothing:antialiased;background-attachment:fixed;"
        "background:radial-gradient(1100px 520px at 50% -8%,rgba(74,222,128,.10),transparent 62%),"
        "radial-gradient(900px 440px at 100% 4%,rgba(56,189,248,.07),transparent 60%),#06090d}"
        "body:before{content:'';position:fixed;inset:0;pointer-events:none;z-index:0;"
        "background-image:linear-gradient(rgba(255,255,255,.028) 1px,transparent 1px),"
        "linear-gradient(90deg,rgba(255,255,255,.028) 1px,transparent 1px);"
        "background-size:46px 46px;"
        "-webkit-mask-image:radial-gradient(ellipse 72% 50% at 50% 0%,#000 18%,transparent 78%);"
        "mask-image:radial-gradient(ellipse 72% 50% at 50% 0%,#000 18%,transparent 78%)}"
        ".wrap{max-width:1080px;margin:0 auto;position:relative;z-index:1}"
        ".hero{display:flex;justify-content:space-between;align-items:center;gap:20px;"
        "flex-wrap:wrap;padding:4px 2px 18px}"
        ".en{font-size:10.5px;letter-spacing:.42em;color:#4ade80;font-weight:700}"
        "h1{font-size:46px;line-height:1;margin:11px 0 9px;letter-spacing:.10em;font-weight:800;"
        "background:linear-gradient(180deg,#ffffff,#8fa1b6);-webkit-background-clip:text;"
        "background-clip:text;color:transparent}"
        ".meta{font-size:11.5px;color:#6b7d92;letter-spacing:.04em}"
        ".rank{display:flex;align-items:center;gap:13px}"
        ".badge{width:64px;height:72px;display:grid;place-items:center;font-size:29px;"
        "font-weight:800;color:#1c1206;"
        "clip-path:polygon(50% 0,100% 25%,100% 75%,50% 100%,0 75%,0 25%);"
        "background:linear-gradient(165deg,#fde68a,#f59e0b 48%,#b45309)}"
        ".rank .rl{font-size:9px;letter-spacing:.3em;color:#8b9aad}"
        ".rank .rv{font-size:15px;font-weight:700;color:#fbbf24;"
        "font-variant-numeric:tabular-nums;margin-top:3px}"
        ".main{display:grid;grid-template-columns:minmax(0,1.15fr) minmax(0,1fr);gap:12px;"
        "margin-bottom:12px}"
        "@media(max-width:820px){.main{grid-template-columns:1fr}.attrs{grid-template-columns:1fr}}"
        ".panel{position:relative;background:linear-gradient(158deg,#101822,#0a0f16);"
        "border:1px solid #1b2534;padding:16px 18px 18px;margin-bottom:12px}"
        ".panel:before,.panel:after{content:'';position:absolute;width:20px;height:20px;"
        "pointer-events:none}"
        ".panel:before{left:-1px;top:-1px;border-left:2px solid #4ade80;border-top:2px solid #4ade80}"
        ".panel:after{right:-1px;bottom:-1px;border-right:2px solid #4ade80;"
        "border-bottom:2px solid #4ade80}"
        ".ph{display:flex;align-items:center;gap:9px;margin:0 0 13px}"
        ".ph:before{content:'';width:3px;height:12px;background:#4ade80;box-shadow:0 0 10px #4ade80}"
        ".ph span{font-size:10.5px;letter-spacing:.24em;color:#6b7d92;font-weight:700}"
        ".kpi{display:flex;flex-direction:column;justify-content:center;padding:4px 4px 0}"
        ".kpi .kl{font-size:10px;letter-spacing:.26em;color:#6b7d92}"
        ".kpi .kv{font-size:58px;line-height:1;font-weight:800;letter-spacing:-2.5px;"
        "font-variant-numeric:tabular-nums;margin:6px 0 2px;"
        "background:linear-gradient(180deg,#ffffff,#6ee7a7 72%,#22c55e);"
        "-webkit-background-clip:text;background-clip:text;color:transparent}"
        ".kpi .unit{font-size:10.5px;color:#5c6a7d;letter-spacing:.14em}"
        ".grid4{display:grid;grid-template-columns:1fr 1fr;gap:9px;margin-top:14px}"
        ".tile{background:#0b1119;border:1px solid #17202c;padding:9px 11px}"
        ".tile span{display:block;font-size:9.5px;letter-spacing:.16em;color:#6b7d92}"
        ".tile b{font-size:18px;font-weight:700;font-variant-numeric:tabular-nums;color:#dbe3ee}"
        ".tile b small{font-size:10px;color:#5c6a7d;font-weight:400}"
        ".tile b.w{color:#fbbf24}"
        ".attrs{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:10px 30px}"
        "@media(max-width:880px){.attrs{grid-template-columns:1fr}}"
        ".attr{display:grid;grid-template-columns:86px 1fr 52px;gap:9px;align-items:center}"
        ".attr .k{font-size:12px;color:#a8b3c2}"
        ".attr .v{text-align:right;font-size:13px;font-weight:700;"
        "font-variant-numeric:tabular-nums;color:#4ade80}"
        ".bar{height:9px;background:#0d151e;position:relative;"
        "clip-path:polygon(0 0,100% 0,calc(100% - 5px) 100%,0 100%)}"
        ".bar i{position:absolute;left:0;top:0;bottom:0;"
        "background:linear-gradient(90deg,#15803d,#4ade80);"
        "box-shadow:0 0 12px rgba(74,222,128,.55)}"
        ".attr.off .k,.attr.off .v{color:#4a5769}"
        ".attr.off .bar{background:none;border:1px dashed #26313f;clip-path:none}"
        ".hint{margin-top:13px;font-size:11px;color:#6b7d92;line-height:1.75}"
        ".hint b{color:#dbe3ee;font-weight:600}.hint .lo{color:#f87171}.hint .hi{color:#4ade80}"
        ".topics{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:13px 26px}"
        "@media(max-width:880px){.topics{grid-template-columns:1fr 1fr}}"
        ".tp .trow{display:flex;justify-content:space-between;align-items:baseline;gap:8px}"
        ".tp .tk{font-size:12px;color:#c3cddb}"
        ".tp .tv{font-size:12.5px;font-weight:700;font-variant-numeric:tabular-nums}"
        ".tbar{height:7px;background:#0d151e;margin:6px 0 5px;"
        "clip-path:polygon(0 0,100% 0,calc(100% - 4px) 100%,0 100%)}"
        ".tbar i{display:block;height:100%}"
        ".tp .tmeta{font-size:9.5px;color:#5c6a7d;letter-spacing:.03em}"
        ".tp.ok .tv{color:#4ade80}.tp.ok .tbar i{background:linear-gradient(90deg,#15803d,#4ade80)}"
        ".tp.mid .tv{color:#fbbf24}"
        ".tp.mid .tbar i{background:linear-gradient(90deg,#a16207,#fbbf24)}"
        ".tp.bad .tv{color:#f87171}"
        ".tp.bad .tbar i{background:linear-gradient(90deg,#991b1b,#f87171)}"
        ".tp.low .tv{color:#6b7d92}.tp.low .tbar i{background:#28323f}"
        "table{width:100%;border-collapse:collapse;font-size:11.5px;margin-top:2px}"
        "th{font-size:9.5px;letter-spacing:.1em;color:#6b7d92;font-weight:600;text-align:left;"
        "padding:7px 5px;border-bottom:1px solid #1b2534;white-space:nowrap}"
        "td{padding:5px 5px;border-bottom:1px solid #131a24;white-space:nowrap}"
        "tbody tr:hover{background:#0f1720}"
        "th.n,td.n{text-align:right;font-variant-numeric:tabular-nums}"
        "td.bad{color:#f87171}"
        "td.up{color:#4ade80}td.down{color:#f87171}"
        "td small{font-size:9.5px;opacity:.72;margin-left:3px;font-weight:400}"
        "td.dim,.dim{color:#6b7d92}"
        "footer{margin-top:24px;padding-top:13px;border-top:1px solid #131a24;"
        "color:#4a5769;font-size:11px;line-height:1.9}"
        "footer a{color:#4ade80;text-decoration:none}"
        "code{background:#131a24;padding:1px 5px;font-size:11px;color:#8fa1b6}"
    )

    P = ["<!DOCTYPE html><html lang=zh-CN><head><meta charset=utf-8>",
         "<meta name=viewport content='width=device-width,initial-scale=1'>",
         "<title>能力概览</title><style>", CSS, "</style></head><body><div class=wrap>"]

    if not ab["n"]:
        P.append("<p>先记录一场比赛，这一页才有内容。</p></div></body></html>")
        return "".join(P)

    mine_all = sum(c["my"] for c in cs)
    ac_all = sum(c["full_count"] for c in cs)
    n_all = sum(c["n_problems"] for c in cs)
    mid_items = [(c, p) for c in cs for p in c["problems"] if 0.30 <= p["rate"] < 0.60]
    mid_zero = sum(1 for _, p in mid_items if p["my"] == 0)
    viol = sum(1 for c in cs for p in c["problems"]
               if p["minutes"] and p["minutes"] > STOP_LOSS and p["status"] != "full")
    ranks = [(c["rank"], c["players"]) for c in cs if c["rank"]]
    best_r, best_pp = min(ranks, key=lambda t: t[0])
    pcts = [r / p for r, p in ranks if p]
    best_pct = min(pcts) if pcts else None

    lo = min(known, key=lambda t: t[2])
    hi = max(known, key=lambda t: t[2])

    tier, tier_txt = _rank_tier(best_pct)
    # 当前积分 = 最近一场结算后的 Rating（不是得分之和）
    rated = [c for c in cs if c.get("rating") is not None]
    cur_rating = rated[-1]["rating"] if rated else None
    cur_delta = rated[-1].get("rating_delta") if rated else None
    delta_txt = (f"{cur_delta:+d}" if isinstance(cur_delta, int) else "—") if rated else ""
    kpi_val = f"{cur_rating}" if cur_rating is not None else f"{mine_all:,.0f}"
    kpi_unit = (f"牛客 RATING · 本场 {delta_txt}" if rated
                else f"ACROSS {ab['n']} CONTESTS")
    P.append(f"<div class=hero><div>"
             f"<div class=en>ALGORITHM PRACTITIONER</div>"
             f"<h1>{PLAYER}</h1>"
             f"<div class=meta>牛客周赛 {ab['n']} 场 · {cs[0]['date']} → {cs[-1]['date']}"
             f" · C++ 主语言</div></div>"
             f"<div class=rank><div class=badge>{tier}</div>"
             f"<div><div class=rl>RANK BY PERCENTILE</div>"
             f"<div class=rv>{tier_txt}</div></div></div></div>")

    P.append("<div class=main>")
    P.append("<section class=panel><div class=ph><span>六维能力</span></div>")
    P.append(_radar_svg(d))
    P.append("</section>")

    P.append("<section class=panel><div class=ph><span>总览</span></div>"
             f"<div class=kpi><span class=kl>当前积分</span>"
             f"<b class=kv>{kpi_val}</b>"
             f"<span class=unit>{kpi_unit}</span></div>"
             f"<div class=grid4>"
             f"<div class=tile><span>完全通过</span><b>{ac_all}<small>/{n_all}</small></b></div>"
             f"<div class=tile><span>中档题零分</span>"
             f"<b>{mid_zero}<small>/{len(mid_items)}</small></b></div>"
             f"<div class=tile><span>止损违规</span><b class=w>{viol}</b></div>"
             f"<div class=tile><span>最佳名次</span><b>#{best_r}"
             f"<small>/{best_pp}</small></b></div>"
             f"</div></section></div>")

    P.append("<section class=panel><div class=ph><span>属性</span></div><div class=attrs>")
    for key, name, _ in DIMS:
        v = d.get(key)
        if v is None:
            P.append(f"<div class='attr off'><span class=k>{name}</span>"
                     "<span class=bar></span><span class=v>未解锁</span></div>")
        else:
            w = min(v, 1.0) * 100
            P.append(f"<div class=attr><span class=k>{name}</span>"
                     f"<span class=bar><i style='width:{w:.1f}%'></i></span>"
                     f"<span class=v>{_pct(v)}</span></div>")
    P.append("</div>")
    P.append(f"<div class=hint>最短板 <b class=lo>{lo[1]} {_pct(lo[2])}</b> · "
             f"最长板 <b class=hi>{hi[1]} {_pct(hi[2])}</b> &nbsp;|&nbsp; "
             f"「独立完成率」未解锁：{n_all} 道训练题都没有「是否独立」标注，<b>不补 0</b></div>")
    P.append("</section>")

    P.append("<section class=panel><div class=ph><span>知识点掌握 · 按题目 tag 聚合</span></div>"
             "<div class=topics>")
    for tname, st in topic_facts(cs):
        v = st["rate"]
        heat = "low" if st["n"] < 2 else ("ok" if v >= 0.8 else ("mid" if v >= 0.4 else "bad"))
        P.append(f"<div class='tp {heat}'><div class=trow><span class=tk>{tname}</span>"
                 f"<span class=tv>{_pct(v)}</span></div>"
                 f"<div class=tbar><i style='width:{(v or 0) * 100:.1f}%'></i></div>"
                 f"<div class=tmeta>{st['n']} 题 · 得分 {st['my']:g}/{st['full']:g}</div></div>")
    P.append("</div>")
    P.append("<div class=hint><b class=hi>绿 ≥80%</b> · <b style='color:#fbbf24'>黄 40–80%</b> · "
             "<b class=lo>&lt;40%</b> · 灰色 = 只有 1 道题，样本不足，不下结论。"
             "一道题可计入多个知识点（如「模拟 + 前缀和」两边都算）。</div>")
    P.append("</section>")

    P.append("<section class=panel><div class=ph><span>逐场记录</span></div><table>"
             "<thead><tr><th>日期</th><th>赛事</th><th class=n>得分</th><th class=n>得分率</th>"
             "<th class=n>AC</th><th class=n>中档</th><th class=n>纪律</th>"
             "<th class=n>积分</th><th class=n>名次</th></tr></thead><tbody>")
    for s in ab["series"]:
        rk = f"{s['rank']}/{s['players']}" if s.get("rank") and s.get("players") else (
            str(s["rank"]) if s.get("rank") else "—")
        rt, rd = s.get("rating"), s.get("rating_delta")
        if rt is None:
            rc = "<td class='n dim'>—</td>"
        else:
            cls = "n up" if (rd or 0) >= 0 else "n down"
            dtx = f"{rd:+d}" if isinstance(rd, int) else ""
            rc = f"<td class={cls}>{rt}<small>{dtx}</small></td>"
        P.append(f"<tr><td class=dim>{s['date'][5:]}</td>"
                 f"<td>{s['name'].replace('牛客周赛 Round ', 'R')}</td>"
                 f"<td class=n>{s.get('my', 0):g}<span class=dim>/{s.get('total', 0):g}</span></td>"
                 f"<td class=n>{_pct(s.get('rate'))}</td>"
                 f"<td class=n>{s.get('full_count', 0)}/{s.get('n_problems', 0)}</td>"
                 f"<td class=n>{_pct(s.get('mid'))}</td>"
                 f"<td class=n>{_pct(s.get('ontime'))}</td>"
                 f"{rc}"
                 f"<td class='n dim'>{rk}</td></tr>")
    P.append("</tbody></table></section>")

    P.append("<section class=panel><div class=ph><span>中档题明细 · 通过率 30–60%</span></div><table>"
             "<thead><tr><th>日期</th><th>题</th><th class=n>通过率</th><th class=n>满分</th>"
             "<th class=n>我的分</th><th class=n>拿分率</th></tr></thead><tbody>")
    t_my = t_full = 0
    for c, p in mid_items:
        t_my += p["my"]
        t_full += p["full"]
        cls = "n bad" if p["my"] == 0 else "n"
        P.append(f"<tr><td class=dim>{c['date'][5:]}</td><td>{p['letter']} {p['title']}</td>"
                 f"<td class=n>{p['rate'] * 100:.1f}%</td><td class=n>{p['full']:g}</td>"
                 f"<td class={cls}>{p['my']:g}</td>"
                 f"<td class=n>{p['my'] / p['full'] * 100:.1f}%</td></tr>")
    if t_full:
        P.append(f"<tr><td class=dim colspan=3>合计</td><td class=n>{t_full:g}</td>"
                 f"<td class=n>{t_my:g}</td>"
                 f"<td class=n>{t_my / t_full * 100:.1f}%</td></tr>")
    P.append("</tbody></table></section>")

    P.append(f"<footer>生成时间 {datetime.now():%Y-%m-%d %H:%M} · 止损线 {STOP_LOSS}′ · "
             f"每个数字都能从 <code>.ai/真实比赛/比赛记录/*.json</code> 复算 · "
             f"逐题档案见 <a href=\"记录.html\">记录.html</a><br>"
             f"「当前积分」= 牛客 Rating，每场结算后的值（取自比赛主页），不是得分之和"
             f"<br>这不是评分：六维全部是比率，分母在 <code>COMPETE.md §6.4 G 段</code>；"
             f"没有数据的一维留空，不补 0<br>"
             f"注：R163（2026-09-27，812 分，742/1056）未留逐题记录，不计入上面的统计，"
             f"但它才是 Rating 曲线的起点</footer></div></body></html>")
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
    ABILITY.write_text(build_ability(cs), encoding="utf-8")

    print(f"[完成] 比赛 {len(cs)} 场" + (f" · 训练 {tr['days']} 天" if tr else " · 训练层为空"))
    print(f"       {OUT.relative_to(ROOT)}/brief.md · snapshot.json · "
          f"{PAGE.relative_to(ROOT)} · {ABILITY.relative_to(ROOT)}")
    if a.open:
        webbrowser.open(PAGE.as_uri())
    return 0


if __name__ == "__main__":
    sys.exit(main())
