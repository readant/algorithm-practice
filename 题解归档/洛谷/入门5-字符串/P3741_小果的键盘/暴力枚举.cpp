// 暴力枚举

// 头文件 iostream：提供 cin、cout 等标准输入输出流
#include <iostream>
// 头文件 string：提供 string 类型，用于存储和操作字符串
#include <string>
using namespace std;

// 统计字符串 s 中 "VK" 出现的次数
// 参数用 const string& 表示引用传递，避免拷贝整个字符串（提高效率），同时 const 保证不会修改原字符串
int countVK(const string& s) {
    int cnt = 0;                         // 计数器
    // 遍历到倒数第二个字符，因为需要检查 s[i] 和 s[i+1]
    for (size_t i = 0; i + 1 < s.size(); ++i) {
        if (s[i] == 'V' && s[i + 1] == 'K') {
            ++cnt;
        }
    }
    return cnt;
}

int main() {
    // 关闭 C 和 C++ 的输入输出同步，加快 cin/cout 速度
    ios::sync_with_stdio(false);
    // 解除 cin 和 cout 的绑定，进一步提速
    cin.tie(nullptr);

    int n;
    string s;
    cin >> n >> s;    // 读取长度和字符串

    // 记录答案，初始为不修改时的 "VK" 数量
    int ans = countVK(s);

    // 枚举每一个位置，尝试修改该位置的字符
    for (int i = 0; i < n; ++i) {
        char original = s[i];   // 备份原字符，用于恢复

        // 将 s[i] 改为另一个字符
        if (original == 'V') {
            s[i] = 'K';
        } else {  // original == 'K'
            s[i] = 'V';
        }

        // 修改后统计，并与当前答案比较，保留较大值
        int cur = countVK(s);
        if (cur > ans) ans = cur;

        // 恢复原字符，保证下一次枚举从原始字符串开始
        s[i] = original;
    }

    // 输出最终答案，用 '\n' 换行，不用 endl（endl 会强制刷新缓冲区，影响性能）
    cout << ans << '\n';
    return 0;
}