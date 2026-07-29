#include <iostream>   // 提供 cin, cout
#include <string>     // 提供 string 类型
#include <cmath>      // 提供 sqrt() 函数（可选，这里用 i*i 代替）
using namespace std;  // 允许直接使用 std 命名空间中的标识符

// 质数判断函数
bool isPrimeNumber(int x) {
    // 小于2的数都不是质数
    if (x < 2) return false;
    // 2 是质数
    if (x == 2) return true;
    // 偶数（大于2）不是质数
    if (x % 2 == 0) return false;
    // 从3开始检查奇数因子，只需要检查到 sqrt(x)
    for (int i = 3; i * i <= x; i += 2) {
        if (x % i == 0) {
            return false;  // 找到因子，不是质数
        }
    }
    return true;  // 没有找到因子，是质数
}

int main() {
    // 加速输入输出（关闭 C 和 C++ 流同步，提高效率）
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string word;
    cin >> word;  // 读入单词，没有空格，所以直接用 cin

    // 第1步：统计每个字母出现次数
    // 使用大小为26的数组，下标0~25对应 'a'~'z'
    // 初始化所有元素为0（在栈上分配，必须显式初始化）
    int cnt[26] = {0};

    // 遍历单词的每个字符
    for (char ch : word) {
        // 计算下标：'a'->0, 'b'->1, ... 'z'->25
        int idx = ch - 'a';
        cnt[idx]++;  // 对应字母出现次数加1
    }

    // 第2步：找出出现过的字母中的最大次数和最小次数
    // minn 初始化为一个很大的数（因为单词长度<100，所以100足够大）
    int maxn = 0;
    int minn = 100;

    for (int i = 0; i < 26; ++i) {
        if (cnt[i] == 0) {
            continue;  // 该字母没出现，跳过
        }
        if (cnt[i] > maxn) {
            maxn = cnt[i];
        }
        if (cnt[i] < minn) {
            minn = cnt[i];
        }
    }

    // 第3步：计算差值
    int ans = maxn - minn;

    // 第4步：判断差值是否为质数并输出
    if (isPrimeNumber(ans)) {
        cout << "Lucky Word\n";   // 使用 '\n' 而非 endl，避免刷新缓冲区
        cout << ans << '\n';
    } else {
        cout << "No Answer\n";
        cout << 0 << '\n';
    }

    return 0;
}