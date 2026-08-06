#include <iostream>   // 提供 cin, cout 等输入输出功能
#include <string>     // 提供 string 类型，用于存储字符串

using namespace std;  // 允许直接使用标准库中的名称（如 cin、cout、string）

// 定义常量，表示取模的除数
const int MOD = 47;

// 函数：计算一个字符串的“编号”
// 参数：const string& s 表示传入的字符串（引用传递，避免拷贝，const 表示不修改）
// 返回值：int 类型，为 s 的字母乘积对 MOD 取模后的结果
int calc(const string& s) {
    int res = 1;  // 乘积的初始值设为 1（因为要连乘）
    // 使用范围 for 循环遍历字符串中的每个字符，简洁且安全
    for (char ch : s) {
        // 将字符转换为数字：'A'->1, 'B'->2, ..., 'Z'->26
        int num = ch - 'A' + 1;
        // 累乘并取模，保证 res 始终小于 MOD
        res = (res * num) % MOD;
    }
    return res;
}

int main() {
    // 加速输入输出，取消 C 和 C++ 的输入输出同步，让 cin/cout 更快
    ios::sync_with_stdio(false);
    // 解除 cin 和 cout 的绑定，避免不必要的刷新，进一步提高效率
    cin.tie(nullptr);

    string comet, group;  // 定义两个 string 变量存放彗星名和小组名

    // 读取两行字符串（由于题目保证不含空格，直接使用 cin >> 即可）
    cin >> comet >> group;

    // 分别计算两个字符串的编号
    int numComet = calc(comet);
    int numGroup = calc(group);

    // 根据比较结果输出对应单词，并换行（使用 '\n' 而非 endl，避免强制刷新缓冲区）
    if (numComet == numGroup) {
        cout << "GO\n";
    } else {
        cout << "STAY\n";
    }

    return 0;  // 程序正常结束
}