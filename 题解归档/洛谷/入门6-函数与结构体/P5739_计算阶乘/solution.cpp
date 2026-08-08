#include <iostream>   // 提供 cin、cout 等输入输出功能

// 递归函数：计算 n!（n 为非负整数）
long long fact(int n) {
    // 终止条件：0! = 1
    if (n == 0) {
        return 1;
    }
    // 递推关系：n! = n * (n-1)!
    return n * fact(n - 1);
}

int main() {
    // 加速 C++ 输入输出流，使 cin/cout 更快（本题数据小，非必须但规范）
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;          // 读取 n
    long long ans = fact(n); // 调用递归函数计算结果
    std::cout << ans << '\n'; // 输出结果，用 '\n' 换行（比 endl 效率高，不强制刷新缓冲区）
    return 0;
}