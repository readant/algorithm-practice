// 牛客周赛 Round 165 · E 小月的折返颜色   （正解 / 并查集缩点 + 计数）
//
// 题意：树 n 个点，颜色 c_i。统计无序点对 {u,v} 的数量，满足 c_u = c_v，且 u->v 的简单路径
//       恰好换色两次（每有一对相邻节点颜色不同记一次换色）。
//
// 关键观察：
//   一条路径的"换色次数" = 路径上"异色边"的条数。把树里所有"同色边"(c_u==c_v) 缩成连通分量
//   （用并查集），得到缩点树 T'，每个分量颜色统一，相邻分量颜色必不同。
//   u,v 之间的换色次数 = T' 中 comp(u) 到 comp(v) 的边数。
//   "恰好 2 次换色且 c_u=c_v" <=> comp(u) 与 comp(v) 是 T' 中某个中间分量 C 的两个邻居，
//   且这两个邻居分量同色（于是两端同色），路径形如 X - Y - X。
//
// 于是答案 = Σ_{每个分量 C} Σ_{颜色 X} Σ_{C 的两个同色 X 邻居 A,B} size(A)*size(B)。
//   对 C 的邻居按颜色分组，设某色组内各邻居分量为 s_1,s_2,...，则 Σ_{i<j} s_i s_j
//   = ((Σ s_i)^2 - Σ s_i^2)/2，可 O(邻居数) 求出。全体合计 O(n)。
//
// 无多重边（原图是树，缩点后仍是树），故不会重复计数。

#include <iostream>        // cin / cout
#include <vector>          // vector
#include <unordered_map>   // unordered_map
#include <utility>         // pair / swap
#include <algorithm>       // swap
using namespace std;

struct DSU {
    vector<int> p, sz;
    DSU(int n) : p(n + 1), sz(n + 1, 1) { for (int i = 0; i <= n; i++) p[i] = i; }
    int find(int x) { while (p[x] != x) { p[x] = p[p[x]]; x = p[x]; } return x; }
    void uni(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return;
        if (sz[a] < sz[b]) swap(a, b);
        p[b] = a; sz[a] += sz[b];
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n;
    if (!(cin >> n)) return 0;
    vector<long long> c(n + 1);
    for (int i = 1; i <= n; i++) cin >> c[i];

    DSU dsu(n);
    vector<pair<int, int>> edges(n > 0 ? n - 1 : 0);
    for (int k = 0; k + 1 < n; k++) {
        int u, v;
        cin >> u >> v;
        edges[k] = {u, v};
        if (c[u] == c[v]) dsu.uni(u, v);
    }

    vector<int> comp(n + 1);
    unordered_map<int, long long> sizeOf, colorOf;
    for (int i = 1; i <= n; i++) {
        comp[i] = dsu.find(i);
        sizeOf[comp[i]] += 1;
        colorOf[comp[i]] = c[i];
    }

    // 缩点树邻接
    unordered_map<int, vector<int>> adjC;
    for (auto& e : edges) {
        int a = comp[e.first], b = comp[e.second];
        if (a != b) { adjC[a].push_back(b); adjC[b].push_back(a); }
    }

    long long ans = 0;
    for (auto& kv : adjC) {
        // 按邻居分量的颜色分组，累计 (S, SQ)
        unordered_map<long long, pair<long long, long long>> g;
        for (int nb : kv.second) {
            long long col = colorOf[nb], s = sizeOf[nb];
            auto& pr = g[col];
            pr.first += s;
            pr.second += s * s;
        }
        for (auto& pr : g) {
            long long S = pr.second.first, SQ = pr.second.second;
            ans += (S * S - SQ) / 2;
        }
    }
    cout << ans << '\n';
    return 0;
}
