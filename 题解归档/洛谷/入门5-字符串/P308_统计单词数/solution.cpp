#include <iostream>   // 提供 cin, cout 输入输出流
#include <string>     // 提供 string 类型和 getline 函数
#include <cctype>     // 提供 tolower 函数（用于字符转小写）

using namespace std;  // 使用标准命名空间，方便调用 cin/cout/string 等

// 将字符串转换为小写（不区分大小写的匹配前提）
string toLowerStr(string s) {
    // & 表示引用，不加 & 表示复制，复制后 s 变量不会影响到原字符串
    for (char &c : s) {          // 范围 for 循环，c 是 s 中每个字符的引用
        c = tolower(c);          // tolower 来自 <cctype>，将大写转小写
    }
    return s;
}

int main() {
    // 输入输出加速，关闭 C 和 C++ 的输入输出同步，提高效率
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string word, article;
    cin >> word;                 // 读取给定单词（不含空格）
    cin.ignore();                // 忽略第一行末尾的换行符，为后面的 getline 做准备

    getline(cin, article);       // 读取整篇文章（可能包含空格）

    // 将给定单词转为小写，方便后续比较
    word = toLowerStr(word);
    int len = word.size();

    int cnt = 0;                 // 匹配次数
    int firstPos = -1;           // 第一次出现位置，初始 -1 表示未找到

    int i = 0;                   // 用于扫描文章的指针
    int n = article.size();

    while (i < n) {
        // 跳过所有空格，定位到下一个单词的起始
        while (i < n && article[i] == ' ') {
            ++i;
        }
        if (i >= n) break;       // 如果已经扫完，退出

        int j = i;               // j 从单词起始位置开始
        while (j < n && article[j] != ' ') {
            ++j;                 // 移动 j 直到遇到空格或末尾
        }

        // 现在 [i, j) 是一个完整的独立单词
        // 先判断长度是否相等，如果不等，不可能匹配
        if (j - i == len) {
            bool match = true;
            // 逐字符比较（都转为小写）
            for (int k = 0; k < len; ++k) {
                if (tolower(article[i + k]) != word[k]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                ++cnt;
                if (firstPos == -1) {
                    firstPos = i;  // 第一次出现的位置就是当前单词起始下标
                }
            }
        }

        i = j;  // 继续从 j 位置扫描（j 现在是空格或末尾）
    }

    // 输出结果
    if (cnt == 0) {
        cout << -1 << '\n';      // 用 '\n' 而不是 endl，避免频繁刷新缓冲区
    } else {
        cout << cnt << ' ' << firstPos << '\n';
    }

    return 0;
}