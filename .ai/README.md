# .ai — 机器层

> **这个目录是给 AI 用的，人不需要看。** 外层只放人看的东西（`随手记/`、`记录.html`、题解与讲义）。
> 全仓唯一的系统说明就是本文件。

## 里面有什么

| 路径 | 放什么 |
| :-- | :-- |
| `真实比赛/比赛记录/` | 每场比赛一个 JSON（成绩、逐题得分与用时） |
| `真实比赛/赛内代码/` · `原始证据/` | 赛场上真正提交的代码；排行榜/题面截图 |
| `训练记录/日志/` | 每天一个 JSON，逐题记（含**是否独立做出来**） |
| `训练记录/原始代码/` · `证据/` | 训练时真正提交的代码；提交记录截图 |
| `输出/brief.md` | **给 AI 读的摘要**（最省 token，优先读这个） |
| `输出/snapshot.json` | 逐题明细（要细节时读） |
| `build.py` | 唯一的脚本 |

## 怎么跑（AI）

```bash
python .ai/build.py            # 汇总 → 更新 .ai/输出/ 与 仓库根 记录.html
python .ai/build.py --open     # 顺便打开网页
```

只读 `.ai/真实比赛/比赛记录/` 与 `.ai/训练记录/日志/`，只写 `.ai/输出/` 与 `记录.html`。零依赖、离线。

## 人怎么记

**只写仓库根的 [`../随手记/`](../随手记/)**，一行一题：`日期 / 平台 / 题号 / 结果 / 是否独立 / 用时`。
写完让 AI「整理随手记」——AI 负责把它转成上面那些 JSON，再跑 `build.py`。
**不要要求本人手写 JSON 或记命令。**

## 记录格式

**比赛** `真实比赛/比赛记录/YYYY-MM-DD_赛事名.json`（可复制 `_模板.json`）
- `contest`：`id / platform / name / date / rated / contest_system`
- `scoring`：`total_possible / my_total / rank / participants`（不知道写 `null`）
- `problems[]`：`letter / title / full_score / pass_rates / my_score / status / post_contest_score / time_spent_minutes`
- `timeline[]`：`stage / minutes / score_gained / kind`（`题` 或 `空转`）
- `status`：`full` / `partial` / `failed` / `unattempted`
- `pass_rates`：`primary_source` + `frozen`(封榜) / `official_final`(终版) / `official_insession`(当场)；
  写 `numerator`/`denominator` 让脚本现算，别写死 `rate`
- **赛后补题分**写 `post_contest_score`，永不并入 `my_total`

**训练** `训练记录/日志/YYYY-MM-DD_平台.json`（可复制 `日志/_模板.json`）
- `date / platform / evidence_level`（强 / 中 / 弱）
- `problems[]`：`id / verdict / attempts / minutes / independent / tags / code_file`
- `verdict`：`AC` / `WA` / `TLE` / `RE` / `未过` / `未做`
- **`independent`**（最重要）：`独立` / `卡住后自己过` / `看题解后过` / `null`（没记）
- 记不清的一律写 `null`，**不要估**

## 指标口径

- **期望分** = 主通过率 × 满分（用封榜通过率；含赛后补题的终版会虚高）
- **难度档**：签到 ≥90% / 简单 60–90% / 中档 30–60% / 困难 <30%
- **独立完成率** = 独立题数 ÷ **有 `independent` 标注的题数**（没标的题不参与）
- **数据可信度**：强 = 平台判定 + 原始代码；中 = 自述 + 截图；弱 = 只有题号

## 诚实规则（别绕过）

1. **比赛 < 3 场，不做「趋势 / 进步 / 连续」这类表述**，只列事实。
2. **没标「是否独立」的题，不能算独立完成** —— 算不出独立完成率就写「不可计算」。
3. 名次人数不知道就写 `null`，不编百分位。
4. `学习模块/`、`题解归档/` 是学习材料，**不代表水平**，不要拿它们下结论。
5. 时间线若为按累计时长重建，要标明。
6. **不要为少量数据建重架构**：加功能前先问「架子配得上内容吗」。
