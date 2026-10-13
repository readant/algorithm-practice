// 牛客周赛 Round 165 · B 小月的配对   （一次遍历统计）
//
// 题意：给定长度 n 的排列 p_1..p_n 与正整数 k，统计满足 (i + p_i) 能被 k 整除的 i 的个数（i 为 1-based）。
//
// 直接读入即算：i 用循环变量，p_i 顺序读入，判 (i + p_i) % k == 0。
// 注意 i + p_i <= 4e5，long long 无溢出风险。

#include <iostream>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    long long n, k;
    if (!(cin >> n >> k)) return 0;
    long long cnt = 0;
    for (long long i = 1; i <= n; i++) {
        long long p;
        cin >> p;
        if ((i + p) % k == 0) cnt++;
    }
    cout << cnt << '\n';
    return 0;
}
