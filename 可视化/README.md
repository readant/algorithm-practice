# 可视化与数据说明

> 全仓**唯一**的系统说明文档。四篇分散的 README 已合并到这里。

## 目录分工

| 目录 | 放什么 | 性质 |
| :-- | :-- | :-- |
| `真实比赛/` | 赛场成绩、赛内原始代码、截图 | 外部可核验 |
| `训练记录/` | 日常训练、**是否独立做出来**、原始代码、截图 | 自己记的，分强/中/弱 |
| `成长数据/` | `brief.md`（给 AI 读的摘要）、`snapshot.json`（逐题明细） | 生成物 |
| `可视化/` | `build.py`（唯一脚本）、`记录.html`（网页） | 生成物 |
| `学习模块/`、`题解归档/` | 讲义、题解 | 学习材料，**不代表水平** |

## 怎么记（你）

**只写根目录 [`../随手记.md`](../随手记.md)**，一行一题，写清 `日期 / 平台 / 题号 / 结果 / 是否独立`。
写完跟 AI 说「整理随手记」，剩下的 AI 做。

## 怎么跑（AI）

```bash
python 可视化/build.py            # 汇总 → 更新 成长数据/ 与 可视化/记录.html
python 可视化/build.py --open     # 顺便打开网页
```

只读 `真实比赛/比赛记录/` 与 `训练记录/日志/`，只写 `成长数据/` 与 `可视化/记录.html`。零依赖、离线。

## 记录格式

**比赛** `真实比赛/比赛记录/YYYY-MM-DD_赛事.json`（可复制 `_模板.json`）

| 字段 | 说明 |
| :-- | :-- |
| `contest` | `id / platform / name / date / rated / contest_system` |
| `scoring` | `total_possible / my_total / rank / participants`（不知道写 `null`） |
| `problems[]` | `letter / title / full_score / pass_rates / my_score / status / post_contest_score / time_spent_minutes` |
| `timeline[]` | `stage / minutes / score_gained / kind`（`题` 或 `空转`） |

- `status`：`full` 完全通过 / `partial` 部分分 / `failed` 提交没过 / `unattempted` 没做
- `pass_rates`：`primary_source` + `frozen`(封榜) / `official_final`(终版) / `official_insession`(当场)，
  写 `numerator`/`denominator` 让脚本现算，别写死 `rate`
- **赛后补题分**写 `post_contest_score`，永不并入 `my_total`

**训练** `训练记录/日志/YYYY-MM-DD_平台.json`（可复制 `日志/_模板.json`）

| 字段 | 说明 |
| :-- | :-- |
| `date / platform / evidence_level` | `evidence_level`：强 / 中 / 弱 |
| `problems[]` | `id / verdict / attempts / minutes / independent / tags / code_file` |

- `verdict`：`AC` / `WA` / `TLE` / `RE` / `未过` / `未做`
- **`independent`**（最重要）：`独立` / `卡住后自己过` / `看题解后过` / `null`（没记）
- 记不清的一律写 `null`，**不要估**

## 指标口径

- **期望分** = 主通过率 × 满分（用封榜通过率，含赛后补题的终版会虚高）
- **难度档**：签到 ≥90% / 简单 60–90% / 中档 30–60% / 困难 <30%
- **独立完成率** = 独立题数 ÷ **有标注的题数**（没标 `independent` 的题不参与）
- **数据可信度**：强 = 平台判定 + 原始代码；中 = 自述 + 截图；弱 = 只有题号

## 诚实规则（别绕过）

1. **比赛 < 3 场，不做「趋势 / 进步 / 连续」这类表述**，只列事实。
2. **没标「是否独立」的题，不能算独立完成**——独立完成率算不出来就写"不可计算"。
3. 名次人数不知道就写 `null`，不编百分位。
4. `学习模块/`、`题解归档/` 里的东西**不代表水平**，不要拿它们下结论。
5. 时间线若为按累计时长重建，要标明。
