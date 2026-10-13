// 牛客周赛 Round 164 · C 小红的gcd构造  （正解 / AC 版）
//
// 题意：给定 x, y，构造正整数 a, b 使 a*b == x*y 且 gcd(a,b) 尽可能大。
//       输出满足 1 <= a, b <= 10^18 的任意一组。
//
// 正解：把 N = x*y 质因数分解（分别分解 x, y 再合并指数即可，因为 x,y <= 1e9），
//       对每个质因子 p^e，把指数"均分"：
//           a *= p^ceil(e/2),  b *= p^floor(e/2)
//       这样 a*b == x*y 恒成立，且 gcd(a,b) = p^floor(e/2) 取到理论最大值
//       （因为 gcd^2 | x*y，gcd 不可能超过 sqrt 级别）。
//
// 关于上界：x,y <= 1e9 => x*y <= 1e18，而 a <= sqrt(x*y) <= 1e9 级别，
//           故 a, b 自动满足 <= 1e18，无需任何额外处理。
//
// 复杂度：筛素数到 sqrt(1e9) ≈ 31623，每组试除，T<=100 轻松通过。

#include <iostream>
#include <vector>
using namespace std;
using ll = long long;
using i128 = __int128;

const int MAXP = 32000;
vector<int> primes;
bool isp[MAXP + 1];

void sieve() {
    for (int i = 2; i <= MAXP; i++) isp[i] = true;
    for (int i = 2; i <= MAXP; i++) if (isp[i]) {
        primes.push_back(i);
        if (1LL * i * i <= MAXP)
            for (long long j = 1LL * i * i; j <= MAXP; j += i) isp[j] = false;
    }
}

// 分解 x（x <= 1e9）：返回质因数与指数
void factor(ll x, vector<pair<ll, int>>& out) {
    for (int p : primes) {
        if (1LL * p * p > x) break;
        if (x % p == 0) {
            int e = 0;
            while (x % p == 0) { x /= p; e++; }
            out.push_back({(ll)p, e});
        }
    }
    if (x > 1) out.push_back({x, 1});
}

i128 ipow(i128 base, int e) {
    i128 r = 1;
    while (e) { if (e & 1) r *= base; base *= base; e >>= 1; }
    return r;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    sieve();

    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        ll x, y;
        cin >> x >> y;

        vector<pair<ll, int>> fx, fy;
        factor(x, fx);
        factor(y, fy);

        // 合并 x, y 的指数
        vector<pair<ll, int>> mg;
        int i = 0, j = 0;
        while (i < (int)fx.size() && j < (int)fy.size()) {
            if (fx[i].first == fy[j].first) {
                mg.push_back({fx[i].first, fx[i].second + fy[j].second});
                i++; j++;
            } else if (fx[i].first < fy[j].first) {
                mg.push_back(fx[i]); i++;
            } else {
                mg.push_back(fy[j]); j++;
            }
        }
        while (i < (int)fx.size()) mg.push_back(fx[i++]);
        while (j < (int)fy.size()) mg.push_back(fy[j++]);

        // 指数均分
        i128 a = 1, b = 1;
        for (auto& pr : mg) {
            ll p = pr.first;
            int e = pr.second;
            int ea = (e + 1) / 2, eb = e / 2;
            a *= ipow(p, ea);
            b *= ipow(p, eb);
        }

        unsigned long long A = (unsigned long long)a;
        unsigned long long B = (unsigned long long)b;
        cout << A << " " << B << "\n";
    }
    return 0;
}
