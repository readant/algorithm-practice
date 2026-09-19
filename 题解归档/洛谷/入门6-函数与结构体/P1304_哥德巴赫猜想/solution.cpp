#include <iostream>   // 提供 cin, cout 等输入输出流
#include <vector>     // 提供 vector 容器，用于动态数组

using namespace std;  // 允许直接使用 std 命名空间中的标识符，方便书写

int main() {
    // 用 ios::sync_with_stdio(false) 和 cin.tie(nullptr) 加速输入输出
    // 这是 C++ 竞赛中的常用优化，关闭 C 风格 IO 同步，减少 cin/cout 的开销
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;   // 读入上界 N

    // 创建布尔标记数组 isPrime，大小 N+1，并用 vector<char> 而非 vector<bool>
    // 原因：vector<bool> 是特化版本，其元素不是真正的 bool 类型，可能会带来意想不到的行为
    // 使用 vector<char> 行为与普通数组一致，内存占用也只多一点点，完全可接受
    vector<char> isPrime(N + 1, true);

    // 0 和 1 不是质数
    isPrime[0] = isPrime[1] = false;

    // 埃氏筛法：从 2 到 N 进行筛选
    for (int i = 2; i <= N; ++i) {
        if (isPrime[i]) {   // 如果 i 是质数
            // 将 i 的倍数标记为合数，从 i*i 开始（更小的倍数已被更小的质数标记过）
            // 注意：当 i 较大时，i*i 可能超过 int 范围，但这里 N ≤ 10000，安全
            for (int j = i * i; j <= N; j += i) {
                isPrime[j] = false;
            }
        }
    }

    // 验证 4 到 N 的所有偶数
    for (int x = 4; x <= N; x += 2) {
        // 从最小的质数 2 开始枚举第一个加数 p
        // 由于要求第一个加数最小，一旦找到就立即输出并跳出内层循环
        for (int p = 2; p <= x / 2; ++p) {
            if (isPrime[p] && isPrime[x - p]) {
                // 输出格式：x=p+(x-p)，使用 '\n' 换行，不用 endl
                // endl 会强制刷新缓冲区，降低性能，这里无交互式需求
                cout << x << '=' << p << '+' << (x - p) << '\n';
                break;  // 找到最小方案，跳出内层循环，继续下一个偶数
            }
        }
    }

    return 0;   // 程序正常结束
}