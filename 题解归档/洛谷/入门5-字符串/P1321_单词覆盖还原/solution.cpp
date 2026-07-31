#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string s;
    cin >> s;
    int n = (int)s.size();

    // mark[i] = true 表示起始位置 i 已被某个单词占用
    vector<char> mark(n, 0);

    int boy = 0, girl = 0;

    for (int i = 0; i < n; ++i) {
        // ---- 处理 boy ----
        int start = -1;
        if (s[i] == 'b') {
            start = i;          // 'b' 是 boy 的第 1 个字符
        } else if (s[i] == 'o') {
            start = i - 1;      // 'o' 是 boy 的第 2 个字符
        } else if (s[i] == 'y') {
            start = i - 2;      // 'y' 是 boy 的第 3 个字符
        }

        if (start >= 0 && start + 2 < n && !mark[start]) {
            boy++;
            mark[start] = 1;
        }

        // ---- 处理 girl ----
        start = -1;
        if (s[i] == 'g') {
            start = i;          // 'g' 是 girl 的第 1 个字符
        } else if (s[i] == 'i') {
            start = i - 1;      // 'i' 是 girl 的第 2 个字符
        } else if (s[i] == 'r') {
            start = i - 2;      // 'r' 是 girl 的第 3 个字符
        } else if (s[i] == 'l') {
            start = i - 3;      // 'l' 是 girl 的第 4 个字符
        }

        if (start >= 0 && start + 3 < n && !mark[start]) {
            girl++;
            mark[start] = 1;
        }
    }

    cout << boy << '\n' << girl << '\n';
    return 0;
}