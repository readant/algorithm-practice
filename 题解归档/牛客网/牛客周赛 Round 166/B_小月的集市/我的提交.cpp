// 牛客周赛 Round 166 · B 题 · 小月的集市
// 赛内得分：100 / 100（AC，提交于 00:14:07）
// 说明：本人赛内提交代码，原样归档，未做任何修改。
//
// 遗留风险（不是本次失分点，但值得记）：
//   1. `pow(10, k)` 走 double，靠 `+1e-8` 兜浮点误差。本题 yu*10^k ≤ 1e9 < 2^53，
//      10^k 也能被 double 精确表示，所以没翻车；换个数量级就会错。
//   2. `yu` 声明成 double 没有必要，它是 `a % b` 的整数余数。

#include <iostream>
#include <cmath>
using namespace std;

int main() {
    int a,b,k;
    cin >> a >> b >> k;
    int one = a / b;
    double yu = a % b;
    int sui = yu * pow(10,k)/b+ 1e-8;

    cout << one << " " << sui << endl;

    return 0;
}
