// 牛客周赛 Round 165 · A 小月的dx分   （签到）
//
// 题意：Critical Perfect 计 3 分、Perfect 计 2 分、Great 计 1 分、Good/Miss 计 0 分。
//       给定五类判定数量 a,b,c,d,e（依次为 CP/Perfect/Great/Good/Miss），求 DX 总分。
//
// 直接 a*3 + b*2 + c 即可（Good/Miss 不计分）。

#include <iostream>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    long long a, b, c, d, e;
    if (!(cin >> a >> b >> c >> d >> e)) return 0;
    cout << a * 3 + b * 2 + c << '\n';
    return 0;
}
