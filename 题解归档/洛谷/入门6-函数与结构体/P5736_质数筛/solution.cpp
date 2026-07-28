#include <iostream>   // 提供 cin, cout 等输入输出流
using namespace std;  // 使用标准命名空间，简化代码书写

// 【1. 结构体定义】把“数组”和“元素个数”打包在一起
// 作用：便于管理数据，以后如果要传参给函数，只需要传一个结构体变量即可
// 通俗来说：就像一个文具盒，把笔（数据）和笔的数量（长度）都装在一起
struct NumberSet {
    int data[105];   // 根据数据范围 n <= 100，多开一点空间防止越界（防御性编程）
    int count;       // 实际存储的数字个数
};

// 【2. 函数封装】判断一个整数 x 是否为质数
// 这是本专题的核心考点：把独立的逻辑块封装成函数，让主函数更清晰
// 参数：int x（值传递，传入一个整数副本，函数内部修改不影响外部）
// 返回值：bool 类型（true 表示是质数，false 表示不是）
bool isPrime(int x) {
    // 第一步：排除 0 和 1（它们不是质数）
    if (x < 2) return false;

    // 第二步：处理偶数（除了 2 以外，偶数都不是质数）
    // 这里提前处理偶数，可以让后续循环只检查奇数，速度翻倍
    if (x == 2) return true;      // 2 是质数，直接返回
    if (x % 2 == 0) return false; // 其他偶数直接排除

    // 第三步：检查奇数因子（从 3 开始，步长为 2）
    // 为什么只用检查到 i * i <= x ？
    // 因为如果 x = a * b，那么 a 和 b 中必有一个 <= sqrt(x)
    // 例如 36 = 4 * 9，4 <= 6，检查到 4 就能发现它不是质数了
    // 这里用 i * i <= x 代替 i <= sqrt(x)，完全避免浮点数精度问题
    for (int i = 3; i * i <= x; i += 2) {
        if (x % i == 0) {
            return false; // 发现能整除的因子，说明是合数
        }
    }

    // 循环结束都没找到因子，说明是质数
    return true;
}

// 【3. 处理函数】（可选，但推荐，进一步体现结构化）
// 作用：遍历结构体中的数据，调用 isPrime 函数，输出质数
// 参数：const NumberSet& nums（const 引用传递）
// 为什么用 const 引用？
//   - 不用值传递（NumberSet nums）：因为结构体包含数组，复制整个结构体会消耗额外内存和时间。
//   - 用引用（&）：可以直接操作原数据，避免拷贝。
//   - 加 const：保证函数内不会意外修改原数据，安全且让编译器能优化。
void printPrimes(const NumberSet& nums) {
    bool first = true; // 控制空格分隔，避免行尾多余空格

    for (int i = 0; i < nums.count; ++i) {
        // 调用 isPrime 函数判断
        if (isPrime(nums.data[i])) {
            if (!first) {
                cout << ' '; // 先输出空格再输出数字
            }
            cout << nums.data[i];
            first = false;
        }
    }
    cout << '\n'; // 输出换行，使用 '\n' 而非 endl，避免强制刷新缓冲区影响性能
}

int main() {
    // 加速输入输出（虽然本题数据量小，但这是良好的编码习惯）
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 定义结构体变量 nums
    NumberSet nums;

    // 读取数字个数，存入结构体的 count 成员
    cin >> nums.count;

    // 读取 n 个数字，存入结构体的 data 数组
    for (int i = 0; i < nums.count; ++i) {
        cin >> nums.data[i];
    }

    // 调用处理函数，输出结果
    printPrimes(nums);

    return 0; // 程序正常结束
}