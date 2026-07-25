#include <iostream>
using namespace std;
// 枚举（双循环） - 布尔数组（标记数组）- 桶模型 - 边界条件处理
int a[105];
bool mark[20001];
// 对于 100% 的数据，3≤n≤100，测验题给出的正整数大小不超过 10,000。
int main() {
    int n;
    cin >> n;
    for (int i = 0; i < n; ++i) cin >> a[i];

    // 以下是卡顿和不足之处

    // 外循环：选择第一个数
    for (int i = 0; i < n; ++i)
    // 内循环：选择第二个数
        for (int j = i + 1; j < n; ++j)
        // 计算两数之和并标记是否出现过
        // 和的下标就是和，现在可以理解为什么布尔数组要开这么大了
        // 10000 + 10000 = 20000 所以布尔数组开到 20001才能安全使用下标20000
            mark[a[i] + a[j]] = true;  

// 其实时至今日，还是没有习惯使用布尔数组
    int ans = 0;
    for (int i = 0; i < n; ++i) // 遍历是为了干啥
    // 发现是：在看把集合的元素值 对照 上面求到的和 出现过就记一次
        if (mark[a[i]]) ans++;

    cout << ans << endl;
    return 0;
}