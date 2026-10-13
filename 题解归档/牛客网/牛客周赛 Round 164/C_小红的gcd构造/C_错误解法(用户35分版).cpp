// 牛客周赛 Round 164 · C 小红的gcd构造  （用户赛场提交版 / 仅 35 分）
//
// 错因：a = gcd(x,y), b = x*y/gcd(x,y) 只保证了 a*b == x*y，
//       并未让 gcd(a,b) 最大。实测该写法仅在约 47.3% 的情况下恰好最优。
//       最小反例：x=1, y=4 -> 本代码给 (1,4) gcd=1，最优为 (2,2) gcd=2。
//
// 正解见同目录 C_小红的gcd构造.cpp（质因数均分）。

#include <iostream>
using namespace std;
typedef long long ll;

ll gcd_ll(ll a, ll b) {
    while (b) { ll t = a % b; a = b; b = t; }
    return a;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        ll x, y;
        cin >> x >> y;
        ll g = gcd_ll(x, y);
        ll a = g, b = x * y / g;   // ← 错在这里
        cout << a << " " << b << '\n';
    }
    return 0;
}
