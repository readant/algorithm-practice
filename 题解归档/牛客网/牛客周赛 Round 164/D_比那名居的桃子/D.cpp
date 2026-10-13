// 牛客周赛 Round 164 · D 比那名居的桃子  （正解 / 贪心）
//
// 题意：有 n 个顾客，第 i 位愿出 a_i 元买 k 个桃子，并需要 b_i(0/1) 个篮子。
//       进货：桃子 x 元/个，篮子 y 元/个；预算 s 元，进货商最多 m 个桃子。
//       求今天最多收入（不计支出）。保证对任意 i 有 k*x + b_i*y < a_i（不亏本）。
//
// 模型：选一个顾客子集 S 服务。约束：
//       桃子总数 k*|S| <= m，成本 k*x*|S| + y*Σ_{i∈S} b_i <= s。
//       收入 = Σ_{i∈S} a_i，最大化。
//
// 贪心：a_i 全正且服务任一人净收益为正，所以能服务尽量多。
//       把顾客按 b_i 分成两组，组内按 a_i 降序。枚举服务 j 个 b=1 的顾客，
//       则 b=1 组取 a_i 前 j 大；剩余预算/桃子决定还能服务几个 b=0 的（取 a_i 前若干大）。
//       所有 j 取最大收入即为答案。复杂度 O(n log n)。

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
using ll = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll x, y, s, m, k;
    int n;
    if (!(cin >> x >> y >> s >> m >> k >> n)) return 0;

    vector<ll> a(n);
    vector<int> b(n);
    for (int i = 0; i < n; i++) cin >> a[i];
    for (int i = 0; i < n; i++) cin >> b[i];

    vector<ll> B0, B1;
    for (int i = 0; i < n; i++) {
        if (b[i] == 0) B0.push_back(a[i]);
        else B1.push_back(a[i]);
    }
    auto desc = [](ll u, ll v) { return u > v; };
    sort(B0.begin(), B0.end(), desc);
    sort(B1.begin(), B1.end(), desc);

    int c0 = (int)B0.size(), c1 = (int)B1.size();
    vector<ll> p0(c0 + 1, 0), p1(c1 + 1, 0);
    for (int i = 0; i < c0; i++) p0[i + 1] = p0[i] + B0[i];
    for (int i = 0; i < c1; i++) p1[i + 1] = p1[i] + B1[i];

    ll kx = k * x;          // 服务一个 b=0 顾客的成本（仅桃子）
    ll kxy = k * x + y;     // 服务一个 b=1 顾客的成本（桃子+篮子）
    ll maxT = m / k;        // 桃子上限决定最多服务人数
    if (maxT > n) maxT = n;

    ll ans = 0;
    ll lim_j = min((ll)c1, maxT);
    for (ll j = 0; j <= lim_j; j++) {
        __int128 cost1 = (__int128)j * kxy;   // 服务 j 个 b=1 的成本
        if (cost1 > s) break;                  // 超预算，j 再大必超
        __int128 rem = (__int128)s - cost1;
        ll max0_budget = (ll)(rem / kx);       // 剩余预算还能服务几个 b=0
        ll max0_peach = maxT - j;              // 剩余桃子额度
        ll t0 = max0_budget;
        if (max0_peach < t0) t0 = max0_peach;
        if (c0 < t0) t0 = c0;
        if (t0 < 0) t0 = 0;
        ll profit = p1[j] + p0[t0];
        if (profit > ans) ans = profit;
    }
    cout << ans << "\n";
    return 0;
}
