// 牛客周赛 Round 165 · F 小月的峰   （正解 / 二分答案 + 区间可达性判定）
//
// 题意：x_i ∈ [l_i, r_i]，要存在 p(2<=p<=n-1) 使 x_1<x_2<...<x_p>x_{p+1}>...>x_n。
//       步幅 = max|x_{i+1}-x_i|。求所有合法方案中步幅的最小值；无解输出 -1。
//
// 二分行幅 B（合法性与 B 单调：B 越大越容易）。判定 feasible(B)：
//   记 L(i) = "以 i 结尾的严格上升链"可行取值集合（步长 ∈[1,B]，每项落在区间内）。可证它是区间：
//        L(1)=[l1,r1];  L(i)=[ max(l_i, aL(i-1)+1), min(r_i, bL(i-1)+B) ]
//   R(i) = "以 i 为首的严格下降链"可行取值集合：R(n)=[ln,rn];
//        R(i)=[ max(l_i, aR(i+1)+1), min(r_i, bR(i+1)+B) ]
//   峰值取 p 可行 <=> L(p) ∩ R(p) != 空（两条链只在 p 处共享一个取值）。
//   存在某 p(1<=p<=n-2, 0-based) 满足即 feasible。
//
//   ⚠️ 关键：一旦某层区间变空（不存在该长度的链），其后所有层都必须保持空
//   （本次对拍就抓到过"空区间后又假性变非空"的 bug）。
//
// 上界：值域 [-1e9,1e9]，最大差 2e9，故 B ∈ [1, 2e9]；B=2e9 仍不可行 => -1。
// 复杂度 O(n log(2e9))。

#include <cstdio>      // scanf / printf
#include <vector>      // vector
#include <algorithm>   // max / min
using namespace std;
typedef long long ll;

int n;
vector<ll> L, R;

bool feasible(ll B) {
    vector<ll> aL(n), bL(n), aR(n), bR(n);
    vector<char> okL(n), okR(n);

    aL[0] = L[0]; bL[0] = R[0]; okL[0] = (aL[0] <= bL[0]);
    for (int i = 1; i < n; i++) {
        if (!okL[i - 1]) { okL[i] = 0; continue; }     // 空必须传播
        aL[i] = max(L[i], aL[i - 1] + 1);
        bL[i] = min(R[i], bL[i - 1] + B);
        okL[i] = (aL[i] <= bL[i]);
    }

    aR[n - 1] = L[n - 1]; bR[n - 1] = R[n - 1]; okR[n - 1] = (aR[n - 1] <= bR[n - 1]);
    for (int i = n - 2; i >= 0; i--) {
        if (!okR[i + 1]) { okR[i] = 0; continue; }
        aR[i] = max(L[i], aR[i + 1] + 1);
        bR[i] = min(R[i], bR[i + 1] + B);
        okR[i] = (aR[i] <= bR[i]);
    }

    for (int p = 1; p <= n - 2; p++) {
        if (!okL[p] || !okR[p]) continue;
        ll lo = max(aL[p], aR[p]), hi = min(bL[p], bR[p]);
        if (lo <= hi) return true;
    }
    return false;
}

int main() {
    if (scanf("%d", &n) != 1) return 0;
    L.resize(n); R.resize(n);
    for (int i = 0; i < n; i++) scanf("%lld %lld", &L[i], &R[i]);

    ll lo = 1, hi = 2000000000LL, ans = -1;
    while (lo <= hi) {
        ll mid = lo + (hi - lo) / 2;
        if (feasible(mid)) { ans = mid; hi = mid - 1; }
        else lo = mid + 1;
    }
    printf("%lld\n", ans);
    return 0;
}
