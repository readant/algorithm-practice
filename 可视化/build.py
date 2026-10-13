#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""build.py — 成长数据的唯一入口（跑一次 = 全套）。

    python 可视化/build.py            # 校验 + 导出数据 + 生成仪表盘（平时只跑这一条）
    python 可视化/build.py --brief    # 只打印给 AI 读的精简摘要
    python 可视化/build.py --check    # 只校验真值源
    python 可视化/build.py --open     # 跑完顺便打开仪表盘

计算逻辑全在 growth_core.py。本文件只做「编排 + 渲染」。
单向、幂等：只读真值源，只写 成长数据/ 与 可视化/dashboard.html，绝不回写真值源。

> 这个脚本是给 AI / 自动化用的。人只需要写 随手记.md，其余交给 AI。
"""

import argparse
import sys
import webbrowser
from datetime import datetime
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import growth_core as gc  # noqa: E402

OUTPUT_HTML = Path(__file__).resolve().parent / "dashboard.html"

C = {
    "bg": "#ffffff", "panel": "#f6f7f9", "panel2": "#eef1f4",
    "fg": "#1f2328", "muted": "#6b7280", "line": "#d8dee4",
    "accent": "#2563eb", "red": "#dc2626", "green": "#16a34a",
    "gray": "#9ca3af", "amber": "#d97706",
}
TIER_COLORS = {"签到": "#93c5fd", "简单": "#60a5fa", "中档": "#2563eb", "困难": "#1e3a8a"}
CONF_ZH = {"high": "高", "medium": "中", "low": "低"}


# --------------------------------------------------------------------------- #
# 渲染工具
# --------------------------------------------------------------------------- #


def esc(s):
    return (str(s).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;"))


def fmt_num(x, nd=1, suffix=""):
    return "—" if x is None else f"{x:.{nd}f}{suffix}"


def fmt_pct(x, nd=1):
    return "—" if x is None else f"{x * 100:.{nd}f}%"


def delta_html(cur, prev, nd=1, suffix="", invert=False):
    if cur is None or prev is None:
        return '<span class="mut">—</span>'
    d = cur - prev
    if abs(d) < 1e-9:
        return '<span class="mut">持平</span>'
    cls = "good" if ((d < 0) if invert else (d > 0)) else "bad"
    return f'<span class="{cls}">{"↑" if d > 0 else "↓"} {abs(d):.{nd}f}{suffix}</span>'


def signin_minutes(c):
    return next((p["time_spent_minutes"] for p in c["problems"] if p["tier"] == "签到"), None)


def short_name(c):
    return c["contest"]["name"].replace("牛客周赛 ", "")


def svg_expected_vs_actual(contest):
    probs = contest["problems"]
    n = len(probs)
    row_h, gap = 30, 10
    label_w, bar_max, pad_r = 152, 400, 96
    w = label_w + bar_max + pad_r
    h = n * (row_h + gap) + 22
    maxv = max((p["full_score"] for p in probs), default=1) or 1
    scale = bar_max / maxv
    out = [f'<svg viewBox="0 0 {w} {h}" width="100%" style="max-width:{w}px" role="img">']
    out.append(f'<line x1="{label_w}" y1="6" x2="{label_w}" y2="{h - 10}" stroke="{C["line"]}" stroke-width="1"/>')
    for i, p in enumerate(probs):
        y = 8 + i * (row_h + gap)
        title = p["title"] or ""
        if len(title) > 9:
            title = title[:9] + "…"
        mark = " ⚠︎" if p.get("tier_border") else ""
        exp_w = max(scale * p["expected_score"], 1.5)
        my_w = scale * p["my_score"]
        out.append(f'<text x="0" y="{y + 20}" font-size="12" fill="{C["fg"]}">{esc(p["letter"])} · {esc(title)}{mark}</text>')
        out.append(f'<rect x="{label_w}" y="{y + 5}" width="{exp_w:.1f}" height="9" rx="2" fill="{C["accent"]}" opacity="0.32"/>')
        if my_w > 0:
            out.append(f'<rect x="{label_w}" y="{y + 18}" width="{my_w:.1f}" height="9" rx="2" fill="{C["accent"]}"/>')
        else:
            out.append(f'<text x="{label_w + 3}" y="{y + 26}" font-size="10" fill="{C["red"]}">零分</text>')
        out.append(f'<text x="{label_w + bar_max + 8}" y="{y + 14}" font-size="11" fill="{C["muted"]}">期望 {p["expected_score"]:.1f}</text>')
        out.append(f'<text x="{label_w + bar_max + 8}" y="{y + 27}" font-size="11" fill="{C["accent"]}">实得 {p["my_score"]:g}</text>')
    out.append("</svg>")
    return "".join(out)


def svg_timeline(contests):
    dur = max((c["contest"].get("duration_minutes") or 120) for c in contests)
    row_h, gap = 34, 10
    label_w, track_w = 130, 560
    w = label_w + track_w + 20
    h = len(contests) * (row_h + gap) + 34
    scale = track_w / dur
    out = [f'<svg viewBox="0 0 {w} {h}" width="100%" style="max-width:{w}px" role="img">']
    for m in range(0, int(dur) + 1, 30):
        x = label_w + m * scale
        out.append(f'<line x1="{x:.1f}" y1="16" x2="{x:.1f}" y2="{h - 8}" stroke="{C["line"]}" stroke-width="1"/>')
        out.append(f'<text x="{x:.1f}" y="11" font-size="10" fill="{C["muted"]}" text-anchor="middle">{m}′</text>')
    for i, c in enumerate(contests):
        y = 24 + i * (row_h + gap)
        out.append(f'<text x="0" y="{y + 20}" font-size="12" fill="{C["fg"]}">{esc(short_name(c))}</text>')
        t = 0.0
        for seg in c.get("timeline", []):
            x = label_w + t * scale
            bw = seg["minutes"] * scale
            if seg.get("kind") == "空转":
                fill, txt_c, opac = C["gray"], C["fg"], "0.35"
            else:
                fill, txt_c, opac = C["accent"], "#ffffff", "1"
            out.append(f'<rect x="{x:.1f}" y="{y}" width="{bw:.1f}" height="26" rx="3" fill="{fill}" opacity="{opac}"/>')
            if bw > 22:
                out.append(f'<text x="{x + bw / 2:.1f}" y="{y + 17}" font-size="11" fill="{txt_c}" text-anchor="middle">{esc(seg["stage"])} {(seg.get("score_gained") or 0):g}</text>')
            t += seg["minutes"]
    out.append("</svg>")
    return "".join(out)


def tier_bar(contest):
    tiers = contest["tiers"]
    total = sum(v["total"] for v in tiers.values()) or 1
    segs = []
    for name, v in tiers.items():
        if v["total"] == 0:
            continue
        pct = v["total"] / total * 100
        segs.append(
            f'<div class="tseg" style="width:{pct:.2f}%;background:{TIER_COLORS[name]}" '
            f'title="{name}：共 {v["total"]} 题，完全通过 {v["full"]} 题">'
            f'<span>{name} {v["full"]}/{v["total"]}</span></div>'
        )
    border_probs = [p["letter"] for p in contest["problems"] if p.get("tier_border")]
    note = (f'<span class="tbnote">⚠️ {"/".join(border_probs)} 题通过率距档位边界 ≤2pp，档位判定存疑</span>'
            if border_probs else "")
    return f'<div class="tbar">{"".join(segs)}</div>{note}'


def training_section(t):
    """训练近况区块。没有数据时如实说明，不假装。"""
    head = ('<h2>训练近况（日常训练真值）</h2>'
            '<p class="sub">来源 <code>训练记录/日志/</code>（由 <code>随手记.md</code> 整理而来）。'
            '<b>独立 AC 率只统计有完成度标注的题</b>——弱证据（只有题号记载、无代码无截图）不参与，'
            '否则刷题量会伪装成能力。</p>')
    if not t or t.get("status") == "empty":
        return head + ('<div class="emptybox">训练记录尚未录入。<br>'
                       '在根目录 <code>随手记.md</code> 里写一行一题（日期 / 平台 / 题号 / 结果 / 是否独立），'
                       '然后让 AI 整理。</div>')

    ep = t["evidence_problems"]
    rate = t["independent_ac_rate"]
    rate_txt = f'{rate * 100:.1f}%' if rate is not None else "不可计算"
    rate_cls = "" if rate is not None else ' class="mut"'
    cards = f'''<div class="cards four">
      <div class="card kpi"><div class="cl">训练日</div><div class="cv">{t["days"]}<span class="unit"> 天</span></div><div class="cd">{esc(t["date_first"])} ~ {esc(t["date_last"])}</div></div>
      <div class="card kpi"><div class="cl">累计题数（AC）</div><div class="cv">{t["ac_count"]}<span class="unit"> / {t["problems_total"]}</span></div><div class="cd">日均 {t["problems_per_day"]} 题</div></div>
      <div class="card kpi"><div class="cl">独立 AC 率</div><div class="cv"{rate_cls}>{rate_txt}</div><div class="cd">基于 {t["scored_count"]} 道有完成度标注的题</div></div>
      <div class="card kpi"><div class="cl">依赖题解率</div><div class="cv">{(f'{t["dependency_rate"] * 100:.1f}%' if t["dependency_rate"] is not None else "不可计算")}</div><div class="cd">看题解/卡住后才过的占比</div></div>
    </div>'''

    n_none = t["problems_total"] - t["scored_count"]
    parts = [("独立", t["independent_count"], "#16a34a"),
             ("卡住后自己过", t["hinted_count"], "#d97706"),
             ("看题解后过", t["solution_dependent_count"], "#dc2626"),
             ("未标注", n_none, "#9ca3af")]
    total = sum(x[1] for x in parts) or 1
    segs = "".join(
        f'<div class="tseg" style="width:{n / total * 100:.2f}%;background:{c}" title="{k}：{n} 题">'
        f'<span>{k[:4]} {n}</span></div>'
        for k, n, c in parts if n > 0)
    comp = (f'<div class="tlabel">独立完成度构成（{t["problems_total"]} 题）</div>'
            f'<div class="tbar">{segs}</div>')

    ev_parts = [("强", ep.get("强", 0), "#16a34a"), ("中", ep.get("中", 0), "#d97706"), ("弱", ep.get("弱", 0), "#9ca3af")]
    ev_total = sum(x[1] for x in ev_parts) or 1
    ev_segs = "".join(
        f'<div class="tseg" style="width:{n / ev_total * 100:.2f}%;background:{c}" title="{k}证据：{n} 题">'
        f'<span>{k} {n}</span></div>'
        for k, n, c in ev_parts if n > 0)
    ev_block = (f'<div class="tlabel">证据分级（{t["problems_total"]} 题）</div>'
                f'<div class="tbar">{ev_segs}</div>')

    warn = ""
    if rate is None:
        warn = (f'<div class="warnbox">⚠️ <b>独立 AC 率不可计算</b>：'
                f'{t["weak_problem_count"]} 道题全部为弱证据，没有完成度标注、没有原始代码。'
                f'这批数据只能证明「做过」，<b>不能</b>支撑「独立做出」的结论。'
                f'要让它变成真证据：① 写随手记时补上「独立 / 看题解」② 把原始代码丢进 '
                f'<code>训练记录/原始代码/</code>。</div>')
    elif t["weak_problem_count"]:
        warn = f'<div class="warnbox">注意：另有 {t["weak_problem_count"]} 道弱证据题未参与独立 AC 率。</div>'

    tag_tbl = ""
    if t["by_tag"]:
        rows = ""
        for k, v in t["by_tag"].items():
            r = v["independent_rate"]
            rate_cell = f"{r * 100:.0f}%" if r is not None else "—"
            rows += (f'<tr><td>{esc(k)}</td><td class="num">{v["independent"]} / {v["scored"]}</td>'
                     f'<td class="num">{rate_cell}</td></tr>')
        tag_tbl = ('<h3 style="font-size:14px;margin:18px 0 6px">按标签的独立 AC 率</h3>'
                   '<table class="cmp"><thead><tr><th>标签</th><th>独立 / 有标注</th><th>独立率</th></tr></thead>'
                   f'<tbody>{rows}</tbody></table>')

    return head + cards + comp + ev_block + warn + tag_tbl


def one_line_verdict(contests, streaks):
    parts = []
    if len(contests) >= 2:
        a, b = contests[-2], contests[-1]
        if a["score_rate"] and b["score_rate"]:
            d = (b["score_rate"] - a["score_rate"]) * 100
            parts.append(f'得分率 {fmt_pct(a["score_rate"])} → {fmt_pct(b["score_rate"])}（{"↑" if d >= 0 else "↓"}{abs(d):.1f}pp）')
    stalled = [s for s in streaks if s["stalled"]]
    if stalled:
        parts.append(f'<b class="bad">但 {"、".join(s["label"] for s in stalled)} 连续多场未变</b>')
    return "；".join(parts) + "。"


# --------------------------------------------------------------------------- #
# 页面组装
# --------------------------------------------------------------------------- #


def render(contests, streaks, training=None):
    last = contests[-1]
    n = len(contests)

    stalled = [s for s in streaks if s["stalled"]]
    if stalled:
        cards = []
        for s in stalled:
            txt, color = gc.stagnation_verdict(s)
            val = ("是" if s["value"] else "否") if isinstance(s["value"], bool) else s["value"]
            cards.append(f'<div class="card stop {color}"><div class="cl">{esc(s["label"])}</div>'
                         f'<div class="cv">{esc(val)}</div><div class="cd">{esc(txt)}</div></div>')
        stagnation_block = (
            '<h2><span class="mk bad">!</span>停滞榜 · 先看这里</h2>'
            '<p class="sub">这些是<b>连续多场没有改变</b>的项。进步项排在它们之后——只看得分率上升，会变成安慰剂。</p>'
            f'<div class="cards">{"".join(cards)}</div>')
    else:
        stagnation_block = '<h2><span class="mk good">✓</span>停滞榜</h2><p class="sub">暂无连续 ≥2 场未变的项。</p>'

    sc = last["scoring"]
    pct_txt = f'{last["percentile"] * 100:.1f}%' if last["percentile"] else '<span class="mut">数据缺失</span>'
    overview = f'''<div class="cards four">
      <div class="card kpi"><div class="cl">得分 / 满分</div><div class="cv">{sc["my_total"]:g} <span class="unit">/ {sc["total_possible"]:g}</span></div><div class="cd">{fmt_pct(last["score_rate"])} 得分率</div></div>
      <div class="card kpi"><div class="cl">名次</div><div class="cv">{sc["rank"] if sc["rank"] else "—"}<span class="unit"> / {sc["participants"] if sc["participants"] else "?"}</span></div><div class="cd">百分位 {pct_txt}</div></div>
      <div class="card kpi"><div class="cl">完全通过</div><div class="cv">{last["solved_full_count"]}<span class="unit"> / {len(last["problems"])} 题</span></div><div class="cd">部分分 {last["partial_count"]} 题</div></div>
      <div class="card kpi"><div class="cl">期望分失分（可达）</div><div class="cv bad">{last["expected_loss_reachable"]:g}</div><div class="cd">仅统计封榜通过率 ≥ {int(gc.REACHABLE_RATE_FLOOR * 100)}%</div></div>
    </div>'''

    if n >= 2:
        a, b = contests[-2], contests[-1]
        rows = [
            ("得分率", fmt_pct(a["score_rate"]), fmt_pct(b["score_rate"]), delta_html(b["score_rate"], a["score_rate"], 1, "pp")),
            ("完全通过题数", str(a["solved_full_count"]), str(b["solved_full_count"]), delta_html(b["solved_full_count"], a["solved_full_count"], 0, " 题")),
            ("部分分题数", str(a["partial_count"]), str(b["partial_count"]), delta_html(b["partial_count"], a["partial_count"], 0, " 题", invert=True)),
            ("中档题闭环", f'{a["midtier_closed"]}/{a["midtier_total"]}', f'{b["midtier_closed"]}/{b["midtier_total"]}', delta_html(b["midtier_closed"], a["midtier_closed"], 0, " 题")),
            ("签到题用时", fmt_num(signin_minutes(a), 1, "′"), fmt_num(signin_minutes(b), 1, "′"), delta_html(signin_minutes(b), signin_minutes(a), 1, "′", invert=True)),
            ("尾段空转", fmt_num(a["idle_tail_minutes"], 1, "′"), fmt_num(b["idle_tail_minutes"], 1, "′"), delta_html(b["idle_tail_minutes"], a["idle_tail_minutes"], 1, "′", invert=True)),
            ("单题止损", "已执行" if a["stop_loss_executed"] else f'未执行（{len(a["stop_loss_violations"])} 次）', "已执行" if b["stop_loss_executed"] else f'未执行（{len(b["stop_loss_violations"])} 次）', ""),
            ("按期望分定序", "是" if a["ordering_followed"] else "否", "是" if b["ordering_followed"] else "否", ""),
        ]
        cmp_block = f'''<h2>双场对比（{esc(a["contest"]["date"])} → {esc(b["contest"]["date"])}）</h2>
        <p class="sub">只有 {n} 场，不说「趋势」——趋势线需 ≥ {gc.TREND_MIN_CONTESTS} 场才有意义。这里只做事实对比。</p>
        <table class="cmp"><thead><tr><th>指标</th><th>{esc(short_name(a))}</th><th>{esc(short_name(b))}</th><th>变化</th></tr></thead><tbody>
        {"".join(f'<tr><td>{esc(r[0])}</td><td class="num">{r[1]}</td><td class="num">{r[2]}</td><td>{r[3]}</td></tr>' for r in rows)}
        </tbody></table>'''
    else:
        cmp_block = ""

    tier_blocks = "".join(
        f'<div class="trow"><div class="tlabel">{esc(c["contest"]["date"])} · {esc(short_name(c))}</div>{tier_bar(c)}</div>'
        for c in contests)
    tiers_block = f'<h2>难度档分布</h2><p class="sub">档位按<b>封榜通过率</b>划分（非自评）：签到 ≥90%，简单 60–90%，中档 30–60%，困难 &lt;30%。</p>{tier_blocks}'

    mhe = last["missing_highest_expected"]
    mhe_txt = (f'<b class="bad">{esc(mhe["letter"])} 题（期望 {mhe["expected_score"]:.1f}）</b>'
               if mhe else '<span class="mut">本场无未拿题</span>')
    ev_block = (f'<h2>期望分 vs 实得（{esc(last["contest"]["name"])}）</h2>'
                f'<p class="sub">期望分 = 封榜通过率 × 满分。浅条高、深条矮 = 「该拿没拿」。'
                f'最可惜（期望分最高的零分题）：{mhe_txt}。</p>'
                f'<div class="chart">{svg_expected_vs_actual(last)}</div>')
    if n >= 2:
        ev_block += (f'<details><summary>展开上一场（{esc(contests[-2]["contest"]["name"])}）</summary>'
                     f'<div class="chart">{svg_expected_vs_actual(contests[-2])}</div></details>')

    gantt_block = (f'<h2>时间线甘特（每场一行）</h2><p class="sub">看时间去哪了。灰色斜纹段 = 零提交空转。</p>'
                   f'<div class="chart">{svg_timeline(contests)}</div>')

    rows = "".join(
        f'<tr><td>{esc(c["contest"]["date"])}</td>'
        f'<td>{"✅ 已执行" if c["stop_loss_executed"] else "❌ 未执行"}</td>'
        f'<td>{fmt_num(c["idle_tail_minutes"], 1, "′")}</td>'
        f'<td>{fmt_num(c["total_minutes_used"], 1, "′")}</td>'
        f'<td>{fmt_num(c["score_per_minute"], 2, " 分/分")}</td>'
        f'<td class="num">{c["expected_loss_reachable"]:g}</td></tr>'
        for c in contests)
    disc_block = f'''<h2>纪律与效率（逐场）</h2>
    <table class="cmp"><thead><tr><th>日期</th><th>单题止损（{gc.STOP_LOSS_LINE_MINUTES}′线）</th><th>尾段空转</th><th>有效用时</th><th>得分速度</th><th>期望分失分</th></tr></thead>
    <tbody>{rows}</tbody></table>'''

    order_rows = "".join(
        f'<div class="orow"><div class="olabel">{esc(c["contest"]["date"])}</div>'
        f'<div class="chips"><span class="chip lead">期望序（非签到）</span>'
        + "".join(f'<span class="chip">{L}</span>' for L in c["ordering_want"])
        + f'</div><div class="chips"><span class="chip lead">实际尝试</span>'
        + "".join(f'<span class="chip {"hit" if L in c["ordering_got"] else "miss"}">{L}</span>' for L in c["ordering_want"])
        + f'</div><div class="verdict {"ok" if c["ordering_followed"] else "no"}">{"按期望分执行 ✅" if c["ordering_followed"] else "未按期望分执行 ❌"}</div></div>'
        for c in contests)
    order_block = f'<h2>定序对照</h2><p class="sub">规则：非签到档中，按期望分降序，实际尝试的题应为该序列的<b>前缀</b>。</p>{order_rows}'

    prov_rows = "".join(
        f'<tr><td>{esc(c["contest"]["date"])}</td><td>{esc(c["contest"]["name"])}</td>'
        f'<td>{esc("；".join(c["provenance"].get("sources", [])))}{"（时间线为重建值）" if c.get("timeline_reconstructed") else ""}</td>'
        f'<td>{CONF_ZH.get(c["provenance"].get("confidence"), "—")}</td></tr>'
        for c in contests)
    prov_block = f'''<h2>数据溯源</h2>
    <p class="sub">本页所有数字只来自 <code>真实比赛/比赛记录/*.json</code> 与 <code>训练记录/日志/*.json</code>。AI 生成的题解/讲义不参与任何计算。</p>
    <table class="cmp"><thead><tr><th>日期</th><th>赛事</th><th>来源</th><th>置信度</th></tr></thead><tbody>{prov_rows}</tbody></table>'''

    trend_note = (f'<p class="sub warn">⚠️ 目前只有 {n} 场，跨场趋势线（需 ≥ {gc.TREND_MIN_CONTESTS} 场）暂不绘制——'
                  f'n 小时画趋势线会误导。</p>') if n < gc.TREND_MIN_CONTESTS else ""
    tstat = (f'{training["days"]} 天 / {training["problems_total"]} 题'
             if training and training.get("status") == "ok" else "尚未录入")
    generated = datetime.now().strftime("%Y-%m-%d %H:%M")

    css = f"""
    :root {{ --bg:{C['bg']};--panel:{C['panel']};--panel2:{C['panel2']};--fg:{C['fg']};
             --muted:{C['muted']};--line:{C['line']};--accent:{C['accent']};--red:{C['red']};
             --green:{C['green']};--gray:{C['gray']};--amber:{C['amber']}; }}
    * {{ box-sizing:border-box; }}
    body {{ margin:0;background:var(--bg);color:var(--fg);
            font:14px/1.6 -apple-system,"Segoe UI","Microsoft YaHei",sans-serif; }}
    .wrap {{ max-width:920px;margin:0 auto;padding:32px 24px 64px; }}
    header h1 {{ font-size:22px;margin:0 0 6px; }}
    header .sub {{ color:var(--muted);margin:0 0 4px; }}
    .verdict {{ margin-top:10px;padding:10px 14px;background:var(--panel);border-left:3px solid var(--accent);
                border-radius:4px;font-size:14px; }}
    h2 {{ font-size:16px;margin:34px 0 6px;padding-bottom:6px;border-bottom:1px solid var(--line); }}
    .sub {{ color:var(--muted);font-size:12.5px;margin:0 0 14px; }}
    .mut {{ color:var(--muted); }} .bad {{ color:var(--red); }} .good {{ color:var(--green); }}
    .sub.warn {{ color:var(--amber); }}
    .mk {{ display:inline-block;width:18px;height:18px;line-height:18px;text-align:center;border-radius:50%;
          font-size:12px;color:#fff;margin-right:6px; }}
    .mk.bad {{ background:var(--red); }} .mk.good {{ background:var(--green); }}
    .cards {{ display:grid;gap:12px;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));margin-bottom:8px; }}
    .card {{ background:var(--panel);border:1px solid var(--line);border-radius:8px;padding:14px 16px; }}
    .card .cl {{ font-size:12px;color:var(--muted);margin-bottom:6px; }}
    .card .cv {{ font-size:24px;font-weight:600;letter-spacing:-0.5px; }}
    .card .cv .unit {{ font-size:13px;font-weight:400;color:var(--muted); }}
    .card .cd {{ font-size:12px;color:var(--muted);margin-top:6px; }}
    .card.stop {{ border-left:4px solid var(--gray); }}
    .card.stop.red {{ border-left-color:var(--red);background:#fef2f2; }}
    .card.stop.gray {{ border-left-color:var(--gray); }}
    .card.stop .cv {{ font-size:30px; }}
    .card.stop.red .cv {{ color:var(--red); }}
    table.cmp {{ width:100%;border-collapse:collapse;font-size:13px;margin-top:4px; }}
    table.cmp th,table.cmp td {{ padding:8px 10px;border-bottom:1px solid var(--line);text-align:left; }}
    table.cmp th {{ color:var(--muted);font-weight:500;background:var(--panel); }}
    table.cmp td.num {{ font-variant-numeric:tabular-nums; }}
    .trow {{ margin-bottom:12px; }}
    .tlabel {{ font-size:12.5px;color:var(--muted);margin-bottom:4px; }}
    .tbar {{ display:flex;height:26px;border-radius:5px;overflow:hidden;border:1px solid var(--line); }}
    .tseg {{ display:flex;align-items:center;justify-content:center;color:#fff;font-size:11px;white-space:nowrap;overflow:hidden; }}
    .tbnote {{ display:block;font-size:11.5px;color:var(--amber);margin-top:4px; }}
    .chart {{ overflow-x:auto;padding:6px 0; }}
    details {{ margin-top:10px; }} summary {{ cursor:pointer;color:var(--accent);font-size:13px; }}
    .orow {{ padding:12px 0;border-bottom:1px solid var(--line); }}
    .olabel {{ font-size:12.5px;color:var(--muted);margin-bottom:6px; }}
    .chips {{ display:flex;align-items:center;gap:6px;flex-wrap:wrap;margin-bottom:4px; }}
    .chip {{ display:inline-block;min-width:22px;text-align:center;padding:2px 8px;border-radius:12px;
            background:var(--panel2);border:1px solid var(--line);font-size:12px; }}
    .chip.lead {{ background:transparent;border:none;color:var(--muted);font-size:12px;min-width:auto; }}
    .chip.hit {{ background:var(--green);color:#fff;border-color:var(--green); }}
    .chip.miss {{ background:var(--red);color:#fff;border-color:var(--red); }}
    .verdict.ok {{ color:var(--green); }} .verdict.no {{ color:var(--red); }}
    footer {{ margin-top:40px;padding-top:14px;border-top:1px solid var(--line);color:var(--muted);font-size:12px; }}
    code {{ background:var(--panel2);padding:1px 5px;border-radius:3px;font-size:12px; }}
    .emptybox {{ background:var(--panel);border:1px dashed var(--line);border-radius:8px;
                padding:16px;color:var(--muted);font-size:13px;line-height:1.8; }}
    .warnbox {{ background:#fffbeb;border:1px solid #fcd34d;border-left:4px solid var(--amber);
               border-radius:6px;padding:12px 14px;font-size:13px;line-height:1.75;margin-top:12px; }}
    """

    return f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>算法成长仪表盘 · 真实数据</title>
<style>{css}</style>
</head>
<body>
<div class="wrap">
  <header>
    <h1>算法成长仪表盘</h1>
    <p class="sub">两个真值源：<code>真实比赛/比赛记录/</code>（{n} 场，{esc(contests[0]["contest"]["date"])} 起）+ <code>训练记录/日志/</code>（{tstat}）· 由 <code>可视化/build.py</code> 生成，勿手改</p>
    <div class="verdict">{one_line_verdict(contests, streaks)}{trend_note}</div>
  </header>

  {stagnation_block}

  <h2>本场总览（{esc(last["contest"]["name"])}）</h2>
  {overview}

  {cmp_block}
  {tiers_block}
  {ev_block}
  {gantt_block}
  {disc_block}
  {order_block}

  {training_section(training)}

  {prov_block}

  <footer>
    生成时间 {generated} · 止损线 {gc.STOP_LOSS_LINE_MINUTES}′ · 期望分失分门槛 通过率 ≥ {int(gc.REACHABLE_RATE_FLOOR * 100)}%<br>
    真值源：<code>真实比赛/比赛记录/</code> + <code>训练记录/日志/</code> · 你只需写 <code>随手记.md</code>，其余交给 AI
  </footer>
</div>
</body>
</html>
"""


# --------------------------------------------------------------------------- #
# 入口
# --------------------------------------------------------------------------- #


def main():
    ap = argparse.ArgumentParser(description="成长数据：校验 + 导出 + 出图（一条命令全做）")
    ap.add_argument("--check", action="store_true", help="只校验真值源")
    ap.add_argument("--brief", action="store_true", help="只打印给 AI 读的精简摘要")
    ap.add_argument("--open", action="store_true", help="生成后打开仪表盘")
    args = ap.parse_args()

    # --check：只校验
    if args.check:
        warnings, errors = gc.check_all()
        for e in errors:
            print(f"[错误] {e}")
        for w in warnings:
            print(f"[警告] {w}")
        if not errors and not warnings:
            print("[通过] 真值源校验无问题。")
        return 1 if errors else 0

    # --brief：只打印摘要（只为 AI 服务）
    if args.brief:
        try:
            snap, paths = gc.export()
        except gc.GrowthError as e:
            print(f"[错误] {e}", file=sys.stderr)
            return 1
        sys.stdout.write(paths["brief"].read_text(encoding="utf-8"))
        return 0

    # 默认：校验 → 导出 → 出图
    try:
        raw = gc.load_contests()
    except gc.GrowthError as e:
        print(f"[错误] {e}", file=sys.stderr)
        return 1
    try:
        snap, paths = gc.export()
    except gc.GrowthError as e:
        print(f"[错误] {e}", file=sys.stderr)
        return 1

    contests = gc.enrich_all(raw)
    streaks = gc.compute_streaks(contests)
    training = snap["training"]
    OUTPUT_HTML.write_text(render(contests, streaks, training), encoding="utf-8")

    tstat = f"训练 {training['days']} 天 / {training['problems_total']} 题" if training.get("status") == "ok" else "训练层为空"
    print(f"[完成] 比赛 {snap['contests_count']} 场 · {tstat}")
    print(f"       数据出口 → {paths['snapshot'].relative_to(gc.REPO_ROOT)} / {paths['brief'].relative_to(gc.REPO_ROOT)}")
    print(f"       仪表盘   → {OUTPUT_HTML.relative_to(gc.REPO_ROOT)}")
    if snap["warnings"]:
        print(f"       ⚠️ {len(snap['warnings'])} 条警告（跑 --check 看详情）")
    if args.open:
        webbrowser.open(OUTPUT_HTML.as_uri())
    return 0


if __name__ == "__main__":
    sys.exit(main())
