// 牛客周赛 Round 164 · F 小红删树(easy)  （正解 / 树形计数 + 换根，q=1 专用）
//
// 题意：n 节点树，每次删一个叶子（连同边）。q=1 次询问：在"最后一次删除必须删掉第 x 条边"
//       的前提下，把树删到只剩 1 个节点的操作方案数，对 998244353 取模。
//       两种方案不同 <=> 某次操作选的叶子不同。
//
// 模型：固定幸存节点 s（最后剩的那个），则删除顺序需满足：每个节点要在其所有"后代"
//       （以 s 为根）删完之后才能删。合法序列数 = (n-1)! / ∏_{v≠s} size(v)。
//       最后一次删除的必是 s 的某个孩子 u，且删 u 时带走边 (s,u) = 边 x。
//       故边 x 连接 a,b，答案 = W(a,b) + W(b,a)，其中 W(s,u) 表示"幸存 s、最后删 u"。
//
// W(s,u) = (n-2)! * chF(s)*chF(u) * fact(size(u)) / (chSZ(s)*chSZ(u)*f(u))  (模意义下)
//   chF(v)=∏_{c为v孩子} f(c),  chSZ(v)=∏_{c为v孩子} fact(size(c)),
//   f(v)=fact(size(v)-1)*chF(v)/chSZ(v)  （v 作为被删节点时的子树内方案数）。
//
// 用一次换根 DFS 即可在 O(n) 内求出每条有向边对应的 W，从而每个询问 O(1) 回答。
// （本题 q=1，但以下代码对 q>=1 同样适用，故与 G 完全一致。）

#include <iostream>   // cin / cout
#include <vector>     // vector
#include <utility>    // pair
using namespace std;
using ll = long long;
const int MOD = 998244353;

int n, q;
vector<vector<pair<int, int>>> adj;   // (邻居, 边id)
vector<int> sz, f, chF, chSZ;
vector<ll> fact;
vector<int> ans;

ll modpow(ll a, ll e) { ll r = 1; a %= MOD; while (e) { if (e & 1) r = r * a % MOD; a = a * a % MOD; e >>= 1; } return r; }
ll inv(ll x) { return modpow(x % MOD, MOD - 2); }

void dfs1(int u, int p) {
    sz[u] = 1; ll cf = 1, csz = 1;
    for (auto& e : adj[u]) {
        int v = e.first;
        if (v == p) continue;
        dfs1(v, u);
        sz[u] += sz[v];
        cf = cf * f[v] % MOD;
        csz = csz * fact[sz[v]] % MOD;
    }
    chF[u] = cf; chSZ[u] = csz;
    f[u] = (ll)fact[sz[u] - 1] * cf % MOD * inv(csz) % MOD;
}

void dfs2(int u, int p) {
    for (auto& e : adj[u]) {
        int v = e.first, id = e.second;
        // W(u, v)：u 为幸存，v 最后删（每个方向的边都要算，含父方向）
        ll W = fact[n - 2];
        W = W * chF[u] % MOD * chF[v] % MOD * fact[sz[v]] % MOD;
        W = W * inv(chSZ[u]) % MOD * inv(chSZ[v]) % MOD * inv(f[v]) % MOD;
        ans[id] = (ans[id] + (int)W) % MOD;
        if (v == p) continue;   // 父方向只统计、不递归，避免回头

        // 换根：u -> v
        int old_sz_u = sz[u], old_sz_v = sz[v];
        ll oldF_u = chF[u], oldSZ_u = chSZ[u], oldf_u = f[u];
        ll oldF_v = chF[v], oldSZ_v = chSZ[v], oldf_v = f[v];

        chF[u] = (ll)chF[u] * inv(f[v]) % MOD;
        chSZ[u] = (ll)chSZ[u] * inv(fact[sz[v]]) % MOD;
        sz[u] = n - sz[v];
        f[u] = (ll)fact[sz[u] - 1] * chF[u] % MOD * inv(chSZ[u]) % MOD;

        chF[v] = (ll)chF[v] * f[u] % MOD;
        chSZ[v] = (ll)chSZ[v] * fact[sz[u]] % MOD;
        sz[v] = n;

        dfs2(v, u);

        // 还原
        sz[u] = old_sz_u; chF[u] = oldF_u; chSZ[u] = oldSZ_u; f[u] = oldf_u;
        sz[v] = old_sz_v; chF[v] = oldF_v; chSZ[v] = oldSZ_v; f[v] = oldf_v;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if (!(cin >> n >> q)) return 0;
    adj.assign(n + 1, {});
    ans.assign(n, 0);
    for (int i = 1; i < n; i++) {
        int u, v; cin >> u >> v;
        adj[u].push_back({v, i});
        adj[v].push_back({u, i});
    }
    fact.resize(n + 1);
    fact[0] = 1;
    for (int i = 1; i <= n; i++) fact[i] = fact[i - 1] * i % MOD;
    sz.assign(n + 1, 0); f.assign(n + 1, 0); chF.assign(n + 1, 0); chSZ.assign(n + 1, 0);
    dfs1(1, 0);
    dfs2(1, 0);
    while (q--) {
        int x; cin >> x;
        cout << ans[x] << "\n";
    }
    return 0;
}
