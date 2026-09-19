# A1 · STL 容器与算法工具箱（蓝桥杯 C++）

> 对标竞赛常用 STL 用法（vector / stack / queue / 优先队列 / set / map / string）。  
> **定位**：备赛块5 主教材——会用、会选、会避坑。  
> **不是** 408 手写结构教材；手写栈/堆原理见深造轨或算法篇。  
> 总览：[00-数据结构总览.md](00-数据结构总览.md) · 路线：[备赛路线.md](备赛路线.md)

---

## 0. STL 是什么

| 组件 | 含义 | 备赛要会到什么程度 |
| :--- | :--- | :--- |
| 容器 Containers | 存数据的结构 | **本文件主体** |
| 算法 Algorithms | sort / lower_bound… | 会 sort + 二分库 |
| 迭代器 Iterators | 泛化指针 | 会 range-for；会 map 的 `it->first` |
| 仿函数 Functors | 可调用对象 | 会 `greater<int>` 小顶堆；会 lambda 简例 |

```cpp
#include <bits/stdc++.h>   // 比赛常用；正式笔记应知道真实头文件
using namespace std;

sort(a.begin(), a.end());                         // 算法
priority_queue<int, vector<int>, greater<int>> pq; // 容器 + 仿函数
vector<int>::iterator it = a.begin();              // 迭代器
```

**权衡**：STL 省时间、可读性高；极卡常时再手写。蓝桥杯绝大多数题直接 STL。

---

## 1. 选型速查（先看这个）

| 场景 | 首选 |
| :--- | :--- |
| 可变长序列、尾增删、当栈用 | `vector` |
| 只需要尾部进出 | `stack` 或 `vector` 当栈 |
| 先进先出、BFS | `queue` |
| 每次取最大/最小、TopK、合并 | `priority_queue` |
| 去重、有序集合、判断出现过 | `set` / `multiset` |
| 键值映射、计数、字符串字典 | `map` / `unordered_map` |
| 整串读入、拼接、切片 | `string` |
| 二元组（坐标、pair 排序） | `pair` / `vector<pair<...>>` |
| 固定长度数组、卡常 | 原生 `int a[N]` |

```text
需要下标随机访问？ → vector / 数组
需要自动排序？     → set / map（红黑树，O(log n)）
只要最快计数且键连续？ → 桶数组 int cnt[N]
BFS 层层扩散？     → queue
维护当前最大值？   → priority_queue
```

---

## 2. vector（动态数组）

**连续存储**，可当「能扩容的数组」。数据在**堆**上，大二维矩阵不会像超大局部数组那样容易爆栈。

### 2.1 构造

```cpp
vector<int> v;                 // 空
vector<int> v(100);            // 100 个 0
vector<int> v(100, 1);         // 100 个 1
vector<int> v = {1, 2, 3};

// 二维：n 行 m 列，初值 -1（矩阵 n*m <= 1e6 时常用）
int n, m;
vector<vector<int>> mat(n, vector<int>(m, -1));

// 每行长度可不同（邻接表常用）
vector<vector<int>> g(n + 1);
```

**不要写**：

```cpp
vector<int> a[100](100, 1);    // 语法错误
```

`vector<int> a[100]` 是「100 个 vector」，可用于链式邻接表；**不是** 100×100 矩阵。

### 2.2 常用操作

| 操作 | 代码 | 复杂度 |
| :--- | :--- | :--- |
| 尾加 | `v.push_back(x)` | 均摊 O(1) |
| 尾删 | `v.pop_back()` | O(1) |
| 下标 | `v[i]` | O(1) |
| 长度 | `v.size()` | O(1) |
| 判空 | `v.empty()` | O(1) |
| 清空 | `v.clear()` | O(n) |
| 改长 | `v.resize(n, val)` | O(n) |
| 首/尾 | `v.front()` / `v.back()` | O(1) |
| 插入/删除 | `v.insert` / `v.erase` | O(n) |

```cpp
for (int i = 0; i < (int)v.size(); i++) { /* ... */ }
for (int x : v) { /* ... */ }
for (auto &x : v) x *= 2;   // 需要改元素时用引用
```

### 2.3 适用情形

- 一般替代普通数组（除非明确卡常）。  
- 矩阵：`n,m ≤ 1e6` 且 `n*m ≤ 1e6` → 必须 `vector<vector<int>>`，不能开 `int mat[1e6][1e6]`。  
- 大量尾插：先 `v.reserve` 或构造时定长，避免反复扩容。

### 2.4 坑

| 坑 | 说明 | 做法 |
| :--- | :--- | :--- |
| size_t 参与乘法 | `a.size()*a.size()` 在 32 位下易溢出变 0 | `(int)v.size()` 或先存 `int n = v.size()` |
| 边写边扩太慢 | 讲义：1e8 次 push_back 明显慢于预分配 | 长度已知就 `vector<int> a(n)` |
| 迭代器失效 | 中途 insert/erase 后旧迭代器作废 | 竞赛里改完重新取 begin |

---

## 3. string

### 3.1 构造与 IO

```cpp
string s1;
string s2 = "awa!";
string s3(10, '6');          // "6666666666"

string s;
cin >> s;                    // 遇空白停
getline(cin, s);             // 读整行（含空格）

// C 字符串转 string
char buf[100];
scanf("%s", buf);
s = buf;
printf("%s", s.c_str());
```

### 3.2 常用操作

| 操作 | 代码 |
| :--- | :--- |
| 下标 | `s[i]`（可写） |
| 比较 | `s1 == s2` |
| 拼接 | `s1 + s2`，`s += "x"` |
| 长度 | `s.size()` / `s.length()` |
| 子串 | `s.substr(pos, len)` |
| 查找 | `s.find(t)`；失败为 `string::npos` |
| 数字互转 | `to_string` / `stoi` / `stoll` |

```cpp
int pos = s.find("awa");
if (pos != string::npos) {
    string part = s.substr(pos, 3);
}
```

### 3.3 坑

| 坑 | 做法 |
| :--- | :--- |
| `cin >> n` 后 `getline` 跳过 | `cin.ignore()` 或 `getchar()` 吞回车 |
| 下标越界 | 循环用 `i < s.size()`，不要 `<=` |
| 长串 TLE | 先 `int len = s.size()` 再循环 |

### 3.4 与备赛块3 关系

字符映射、凯撒、桶计数等见语法层字符串与相关专题；本节只保证 **string 当容器会用**。

---

## 4. stack（栈）

`#include <stack>`  
**后进先出**。不可下标、不可 range-for 遍历内部。

| 操作 | 代码 |
| :--- | :--- |
| 进栈 | `st.push(x)` |
| 出栈 | `st.pop()`（不返回值） |
| 栈顶 | `st.top()` |
| 判空/大小 | `st.empty()` / `st.size()` |
| 清空 | `while (!st.empty()) st.pop();` |

```cpp
stack<int> st;
st.push(1);
int t = st.top(); st.pop();
```

**适用**：括号匹配、括号/表达式、单调栈（扩展）、DFS 非递归。  
**vector 当栈**：`push_back` / `pop_back` / `back`。

**坑**：`st[i]`、`for (auto x : st)` 均非法。

---

## 5. queue（队列）

`#include <queue>`  
**先进先出**。BFS 核心容器。同样不可下标遍历。

| 操作 | 代码 |
| :--- | :--- |
| 进队 | `q.push(x)` |
| 出队 | `q.pop()` |
| 队首/队尾 | `q.front()` / `q.back()` |
| 判空/大小 | `q.empty()` / `q.size()` |

```cpp
queue<pair<int, int>> q;
q.push({0, 0});
auto [x, y] = q.front(); q.pop();   // C++17
```

**适用**：BFS、滑动窗口单调队列（扩展）、模拟排队。

---

## 6. priority_queue（优先队列 / 堆）

`#include <queue>`  
默认**大顶堆**；底层二叉堆。只可靠堆顶。

| 写法 | 含义 |
| :--- | :--- |
| `priority_queue<int>` | 大顶 |
| `priority_queue<int, vector<int>, greater<int>>` | 小顶 |

| 操作 | 复杂度 |
| :--- | :--- |
| `push` / `pop` | O(log n) |
| `top` | O(1) |

```cpp
priority_queue<int> big;
priority_queue<int, vector<int>, greater<int>> small;
big.push(3); int t = big.top(); big.pop();
```

**适用**：每次要最大/最小、TopK、Dijkstra、合并果子类。

**坑**：

- 只能读 `top`，不能 `pq[i]`、不能改 `top() = x`。  
- 要改堆顶：`auto t = pq.top(); pq.pop(); pq.push(t+1);`。

---

## 7. set / multiset

`#include <set>`  
**有序**集合；底层红黑树。增删查 **O(log n)**。无下标。

| 操作 | 说明 |
| :--- | :--- |
| `insert` | 插入 |
| `erase` | 按值删 |
| `find` | 返回迭代器，失败 `end()` |
| `count` | 0/1（set） |
| 遍历 | 自动从小到大 |

```cpp
set<int> st;
st.insert(5);
if (st.count(3)) { /* 存在 */ }
for (int x : st) cout << x << ' ';
```

| | set | multiset | unordered_set |
| :--- | :---: | :---: | :---: |
| 互异（不重复） | ✔ | 可重复 | ✔ |
| 有序 | 有 | 有 | 无 |

**适用**：

- 去重排序  
- 元素是否出现过（值域过大开不了 vis 数组时）  
- 动态最小/最大（也可用 pq）

**坑**：

- 不能 `st[0]`。  
- 元素**只读**：要改就 `erase` 再 `insert`。  
- 迭代器不能 `it - st.begin()` 求下标。

`find` 与 `count`：存在性优先 `count` 或 `find != end()`。

---

## 8. map / multimap

`#include <map>`  
**有序键值对**；键 O(log n)。底层红黑树。

```cpp
map<string, int> cnt;
cnt["apple"]++;              // 不存在则先插入默认 0 再 ++
map<int, int> mp;
mp[1] = 2;

for (auto &[k, v] : mp)      // C++17
    cout << k << ' ' << v << '\n';

// 旧式
for (auto it = mp.begin(); it != mp.end(); ++it)
    cout << it->first << ' ' << it->second << '\n';

if (mp.count(3)) { /* 键存在 */ }
auto it = mp.find(3);
if (it != mp.end()) { /* it->second */ }
mp.erase(3);
```

| 操作 | 复杂度 |
| :--- | :--- |
| `mp[k]` 增/改/查 | O(log n) |
| `find` / `count` / `erase` | O(log n) |

**适用**：字符串计数、键映射、需要按键有序遍历。

**坑（高频）**：

```cpp
map<char, int> mp;
mp.count('a');     // 0
mp['a'];           // 虽然没赋值，但 [] 会插入 a -> 0
mp.count('a');     // 变成 1 了！
```

- **只判断是否存在**：用 `count` / `find`，不要先 `mp[k]`。  
- 迭代器同样不能相减当下标。

`unordered_map`：平均 O(1)，无序；蓝桥杯可用，竞赛里 map 更稳、可有序输出。讲义主推掌握 **map**。

---

## 9. pair 与常用小件

```cpp
pair<int, int> p = {1, 2};
p.first; p.second;

vector<pair<int, int>> v;
v.push_back({3, 1});
sort(v.begin(), v.end());   // 先比 first 再比 second

tuple / structured bindings 可了解；备赛 pair 足够覆盖多数题。
```

**结构体排序**（接块4）：

```cpp
struct Node { int a, b; };
bool cmp(Node x, Node y) {
    if (x.a != y.a) return x.a > y.a;  // 注意严格弱序，用 < > 不用 <=
    return x.b < y.b;
}
sort(arr, arr + n, cmp);

// 或 lambda
sort(v.begin(), v.end(), [](auto &x, auto &y) {
    return x.a != y.a ? x.a < y.a : x.b > y.b;
});
```

---

## 10. 算法头最小集

`#include <bits/stdc++.h>` 可全开；下面写真实来源便于理解。

| 算法 | 头文件 | 备赛用途 |
| :--- | :--- | :--- |
| `sort` | `<algorithm>` | 几乎每题 |
| `lower_bound` / `upper_bound` | `<algorithm>` | 有序上二分 |
| `reverse` / `unique` / `swap` | `<algorithm>` | 去重套路 |
| `min` / `max` / `abs` | `<algorithm>` / `<cmath>` / `<cstdlib>` | — |
| `memset` | `<cstring>` | 填 0/-1 数组 |

```cpp
sort(a, a + n);                 // 原生数组
sort(v.begin(), v.end());       // vector
int *p = lower_bound(a, a + n, x);  // 第一个 >= x
int idx = int(p - a);
```

**unique 去重**（需先 sort）：

```cpp
sort(v.begin(), v.end());
v.erase(unique(v.begin(), v.end()), v.end());
```

二分答案手写 while 模板见 [03-二分查找](../../03-基础算法体系/03-二分查找.md)（讲义强调二分答案高频，勿只依赖库函数）。

---

## 11. 迭代器与 range-for（够用即可）

```cpp
for (int x : v) {}                    // vector/set
for (auto &pr : mp) pr.second++;      // map
for (auto it = st.begin(); it != st.end(); ++it) {}
```

| 容器 | 可下标 | 可修改元素 |
| :--- | :---: | :--- |
| vector | 是 | 是 |
| string | 是 | 是 |
| set/map | 否 | 否（先 erase/insert） |
| stack/queue/pq | 否 | 否 |

---

## 12. 综合例题骨架（自己补输入）

### 12.1 计数（map）

```cpp
int n; string s;
cin >> n;
map<string, int> cnt;
while (n--) { cin >> s; cnt[s]++; }
for (auto &[k, v] : cnt) cout << k << ' ' << v << '\n';
```

### 12.2 TopK 小（大顶堆）

```cpp
int n, k; cin >> n >> k;
priority_queue<int> pq;   // 保留最大的 k 个：堆里放候选
for (int i = 0, x; i < n; i++) {
    cin >> x;
    pq.push(x);
    if ((int)pq.size() > k) pq.pop();  // 弹出当前最大
}
// 堆中即较小的 k 个（无序）；若要排序再倒入 vector
```

### 12.3 去重统计（set + map）

```cpp
set<int> uniq;
map<int, int> cnt;
int x;
while (cin >> x) { uniq.insert(x); cnt[x]++; }
```

### 12.4 BFS 框架（queue）

```cpp
queue<pair<int, int>> q;
q.push({sx, sy});
vis[sx][sy] = 1;                 // 入队立刻标记
int dx[] = {0, 0, 1, -1};
int dy[] = {1, -1, 0, 0};
while (!q.empty()) {
    auto [x, y] = q.front(); q.pop();
    for (int d = 0; d < 4; d++) {
        int nx = x + dx[d], ny = y + dy[d];
        if (nx < 0 || ny < 0 || nx >= n || ny >= m) continue;
        if (g[nx][ny] == 1 || vis[nx][ny]) continue;
        vis[nx][ny] = 1;         // 入队标记，勿出队才标
        q.push({nx, ny});
    }
}
```

完整 BFS/DFS 见 [06-搜索算法](../../03-基础算法体系/06-搜索算法（DFS&BFS）.md)。

---

## 13. 坑总表（备赛必背）

| 容器 | 红线 |
| :--- | :--- |
| vector | `size()` 转 int；预分配；大矩阵勿静态多维 |
| string | getline 前吞回车；`i < len` |
| stack/queue/pq | **禁止下标与 range-for 内部** |
| pq | 只读 top；改 top 要 pop+push |
| set/map | 只读元素；无下标；map 的 `[]` 会插入 |
| 全部 | 竞赛 I/O 快时可注意 `ios::sync_with_stdio(false)` |

---

## 14. 与仓库其它层的链接

| 需求 | 去处 |
| :--- | :--- |
| API 速查小抄 | [01-参考层/01-C++/01-STL容器速查.md](../../../01-参考层/01-C++/01-STL容器速查.md) |
| 手写栈队列原理（408） | 深造轨 `03-栈与队列.md`（迁移后） |
| 单调栈/队列 | [10-单调栈与单调队列](../../03-基础算法体系/10-单调栈与单调队列.md) |
| 并查集 | [13-并查集](../../03-基础算法体系/13-并查集.md) |
| 模板粘贴 | [01-基础数据结构模板](../../../03-实战层/01-算法模板库/01-基础数据结构模板.md) |
| 踩坑归档 | [03-数组与容器踩坑](../../../03-实战层/02-踩坑记录/03-数组与容器踩坑.md) |

---

## 15. 自测清单

- [ ] 闭卷写出 set 小顶堆？不——**优先队列小顶堆**两行  
- [ ] `mp['a']` 与 `mp.count('a')` 差别能讲清  
- [ ] BFS 为什么入队时 vis  
- [ ] 为什么 `vector<vector<int>>` 能开大矩阵而 `int a[1e6][1e6]` 不能  
- [ ] 结构体 `cmp` 为什么不能写 `<=`  
- [ ] 对每个「适用情形」各想 1 道可套用的题型  

---

## 小结

块5「C++数据结构」= **本工具箱**：vector / string / stack / queue / pq / set / map / pair + 最小算法集 + 坑表。  
学完直接服务：高精度（vector）、BFS（queue）、贪心二分（sort）、真题计数映射（map/set）。  
深造 408 时再补手写结构与复杂度证明；**备赛期以本文件为纲**。
