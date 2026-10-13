// 牛客周赛 Round 165 · A 小月的dx分   （签到）
//
// 题意：Critical Perfect 计 3 分、Perfect 计 2 分、Great 计 1 分、Good/Miss 计 0 分。
//       给定五类判定数量 a,b,c,d,e（依次为 CP/Perfect/Great/Good/Miss），求 DX 总分。
//
// 直接 a*3 + b*2 + c*1 即可（Good/Miss 不计分）。

#include <cstdio>
using namespace std;

int main() {
    long long a, b, c, d, e;
    if (scanf("%lld %lld %lld %lld %lld", &a, &b, &c, &d, &e) != 5) return 0;
    printf("%lld\n", a * 3 + b * 2 + c);
    return 0;
}
