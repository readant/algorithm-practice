// 牛客周赛 Round 164 · G 小红删树(hard)  （正解 / 树形计数 + 换根，多询问）
//
// 与 F 完全相同：求"最后一次删除必须删掉第 x 条边"时的删树方案数。
// 区别仅在于 q 可达 n（多组询问），换根预处理后每组 O(1) 回答。
// 算法与公式见 F.cpp 注释（两题代码一致，仅 q 的范围不同）。

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
