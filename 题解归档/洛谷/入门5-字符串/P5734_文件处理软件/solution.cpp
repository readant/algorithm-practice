#include <iostream>   // 提供 cin、cout 等标准输入输出流
#include <string>     // 提供 string 类型及其成员函数

using namespace std;  // 使用标准命名空间，方便直接使用 cout、string 等

int main() {
    // 关闭 C 与 C++ 的输入输出同步，加快 cin/cout 速度
    ios::sync_with_stdio(false);
    cin.tie(nullptr); // 解除 cin 与 cout 的绑定，进一步提速

    int q;
    cin >> q;         // 读取操作次数

    string doc;
    cin >> doc;       // 读取初始文档（无空格）

    while (q--) {     // 循环 q 次
        int op;
        cin >> op;    // 读取操作类型

        if (op == 1) {
            string str;
            cin >> str;              // 读取要追加的字符串
            doc.append(str);         // 将 str 追加到 doc 末尾
            cout << doc << '\n';     // 输出结果，用 '\n' 换行
        }
        else if (op == 2) {
            int a, b;
            cin >> a >> b;           // 读取起始位置和长度
            doc = doc.substr(a, b);  // 截取从 a 开始的 b 个字符，替换原文档
            cout << doc << '\n';
        }
        else if (op == 3) {
            int a;
            string str;
            cin >> a >> str;         // 读取插入位置和插入字符串
            doc.insert(a, str);      // 在 doc 的第 a 个字符前插入 str
            cout << doc << '\n';
        }
        else if (op == 4) {
            string str;
            cin >> str;              // 读取要查找的子串
            size_t pos = doc.find(str); // 查找首次出现位置，返回 size_t 类型
            if (pos == string::npos) {  // string::npos 是 size_t 的最大值，表示未找到
                cout << -1 << '\n';
            } else {
                cout << pos << '\n';    // 位置从 0 开始，符合题目要求
            }
        }
    }

    return 0;
}