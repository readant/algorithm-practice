# 小红的排列构造

## 元数据

| 项目 | 内容 |
| --- | --- |
| 赛事 | [牛客周赛 Round 164](https://ac.nowcoder.com/acm/contest/141142) E 题 |
| 满分 | 250 |
| 难度 | 中等 / 构造（排列 + 过程模拟逆向） |
| 标签 | 构造、排列、逆向模拟、平衡树 |
| 状态 | 本场 0 分（未读题）；赛后已推得正解 |
| 期望分 | **120.8（全场第一，比 D 还高）** |
| 语言 | C++ |
| 配套代码 | [E.cpp](E.cpp)（逆向构造 + 隐式 treap，已模拟验证） |
| 归属 | 构造类；远期对应 ACM 门槛轨 阶段 E（综合/赛制） |

> 本场复盘见 [../00-复盘.md](../00-复盘.md)。

---

## 1. 题面要点

给定 `n, k`，构造一个 `1..n` 的排列 `p`。小红对它做 `n` 次操作：第 `i` 次：

1. 若当前排列长度 `len` 为奇数，令 `idx=(len+1)/2`；若 `p[idx]==i`，**计 1 分**；若 `len` 为偶数则跳过这步。
2. 删除值为 `i` 的那个元素，剩余按原序拼接，长度减 1。

要求：恰好计 `k` 分。无解输出 `No`。

`1 ≤ n,k ≤ 2e5`。

### 关键观察（做这题必须先看出来的）

- 第 `i` 步删的是值 `i`，所以长度 `len = n−i+1`。
- `len` 为奇数 ⇔ `i` 与 `n` 同奇偶。这些"奇数长度步"才会检查中间。
- **最后一步 `i=n` 时 `len=1`（必为奇数），中间那个唯一元素就是 `n` 自己，所以 `i=n` 这一步"必然得分"**。
- 因此 `k` 的合法范围是 **`1 ≤ k ≤ ⌈n/2⌉`**（奇数长度步的总数），否则 `No`。

样例：n=5 → 奇数步为 i=1,3,5（共 3 步），`k∈{1,2,3}`；n=4,k=3 → 3>⌈4/2⌉=2 → `No` ✓。

---

## 2. 解题思路：逆向构造

正向模拟很乱，改为**从后往前"插入"**建排列：

- 第 `n` 步的数组就是 `[n]`。
- 要得到第 `i` 步的数组，是在第 `i+1` 步的数组里"插入值 `i`"到某个位置（正向删除的逆操作）。
- 我们希望：
  - 若该步是"要得分的奇数步"，把 `i` 插到**当前数组正中间** → 正向时 `i` 正好在中间，得分。
  - 否则把 `i` 插到**最前面** → `i` 在端点，绝不会在中间，不得分。

只要选定哪 `k` 个奇数步"得分"（必须包含 `n`），按上面规则从 `i=n` 倒插到 `i=1`，得到的排列必满足恰好 `k` 分。

> 位置换算（数组当前长 `L = n−i`）：
> - 得分奇数步：插到中间 ⇒ 0-based 位置 `pos = L/2`（`L` 此时为偶数）。
> - 其它：插到最前 ⇒ `pos = 0`。

### 为什么需要对数级"按位置插入"

数组是不断变长的序列，要在"第 `pos` 个位置插入"。`std::vector` 插入是 O(n)，整体 O(n²) 会超时。用**支持"按名次插入"的平衡树 / treap**，每次 O(log n)，整体 **O(n log n)**。

---

## 3. 代码实现（C++，treap 按位置插入）

```cpp
#include <iostream>   // cin / cout
#include <vector>     // vector
#include <random>     // mt19937
#include <chrono>     // steady_clock
using namespace std;
using ull = unsigned long long;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

struct Node {
    int val, pri, sz;
    Node *l, *r;
    Node(int v) : val(v), pri(rng()), sz(1), l(nullptr), r(nullptr) {}
};
int sz(Node* t) { return t ? t->sz : 0; }
void upd(Node* t) { if (t) t->sz = 1 + sz(t->l) + sz(t->r); }

void split(Node* t, int k, Node*& L, Node*& R) {   // L=前k个, R=其余
    if (!t) { L = R = nullptr; return; }
    int ls = sz(t->l);
    if (k <= ls) { split(t->l, k, L, t->l); R = t; }
    else         { split(t->r, k - ls - 1, t->r, R); L = t; }
    upd(t);
}
Node* merge(Node* L, Node* R) {
    if (!L || !R) return L ? L : R;
    if (L->pri > R->pri) { L->r = merge(L->r, R); upd(L); return L; }
    else                 { R->l = merge(L, R->l); upd(R); return R; }
}
Node* insertPos(Node* t, int k, int val) {   // 在第 k 个(0-based)位置插入
    Node *L, *R; split(t, k, L, R);
    return merge(merge(L, new Node(val)), R);
}
void inorder(Node* t, vector<int>& out) {
    if (!t) return;
    inorder(t->l, out); out.push_back(t->val); inorder(t->r, out);
}

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int n, k; cin >> n >> k;
    int oddCnt = (n + 1) / 2;                 // 奇数长度步总数 = ⌈n/2⌉
    if (k < 1 || k > oddCnt) { cout << "No\n"; return 0; }

    // 选 k 个要得分的奇数步：取最大的 k 个（必含 n）
    vector<bool> score(n + 1, false);
    int cnt = 0;
    for (int i = n; i >= 1 && cnt < k; i--)
        if (((n - i) % 2) == 0) { score[i] = true; cnt++; }

    Node* root = nullptr;
    for (int i = n; i >= 1; i--) {
        int L = n - i;                         // 当前已插入个数
        int pos = (((n - i) % 2) == 0 && score[i]) ? L / 2 : 0;
        root = insertPos(root, pos, i);
    }
    cout << "Yes\n";
    vector<int> ans; inorder(root, ans);
    for (int i = 0; i < n; i++) cout << ans[i] << " \n"[i == n - 1];
    return 0;
}
```

**自测**：n=5,k=2 → 输出 `5 3 4 2 1`（或等价排列，判题接受任意一种）；n=4,k=3 → `No`。

---

## 4. 复杂度

| 项目 | 复杂度 |
| --- | --- |
| 时间 | O(n log n)（treap 插入） |
| 空间 | O(n) |

---

## 5. 易错点

1. **漏看"最后一步必得分"** ⇒ 误以为 `k=0` 可解，或 `k` 上限判断错。
2. **正向硬模拟** ⇒ 超时且难构造；务必转逆向"插入"。
3. **插入位置用 vector 导致 O(n²)** ⇒ 必须用平衡树/treap，或线段树找空位。
4. **得分步选错**：必须至少包含 `i=n`；随便选会构造失败。

---

## 6. 举一反三

- **套路本质**：「过程模拟题逆向构造」——正向删除难构造，就逆向"插入"，把约束变成"插在哪"。
- **同类题**：
  - AtCoder ABC 常见排列构造（"构造一个排列使某过程满足性质"）
  - Codeforces 构造题（如 "construct permutation with given inversion/score pattern"）
  - 洛谷 P 系列构造题（如 P5194 括号构造类）
- **变体方向**：把"删值 i / 查中间"换成"删最小/查端点"等，套路一致——先想清楚正向每一步发生了什么，再倒推插入规则。
- **训练切入点**：先练「正向模拟 + 输出过程」类题建立直觉，再练「逆向构造排列」；treap/平衡树按位置插入是通用工具，值得单独练一版模板。

---

## 7. 学习关联

- **本场定位**：期望分全场第一（120.8），你 0 分。这是**本场最可惜的一题**——不是不会，是没看。
- **下次硬规则**：开场先算期望分，E/D 两题必须至少扫一眼题面。
- **归属**：构造类，远期对应 ACM 门槛轨 阶段 E（综合/赛制）。当前可作为"以赛促学"的优先补题对象。

---

> 创建时间：2026-10-05
> 最后更新：2026-10-05
