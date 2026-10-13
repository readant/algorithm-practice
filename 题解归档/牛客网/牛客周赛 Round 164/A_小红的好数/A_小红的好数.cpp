// 牛客周赛 Round 164 · A 小红的好数
// 题意：找一个 n 位正整数，使其个位 == 其平方的个位（"好数"）。
// 构造：首位 '1' + 后面 n-1 个 '0' => 形如 10^(n-1)。
//   10^(n-1) 个位为 0，平方后个位仍为 0，必然满足。
// 数据范围 n<=8，直接字符串输出即可（也可算 10^(n-1) 整数）。

#include <iostream>
#include <string>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    string res(n, '0');
    res[0] = '1';           // 第一位放 1，其余为 0 => 100..0
    cout << res << '\n';
    return 0;
}
