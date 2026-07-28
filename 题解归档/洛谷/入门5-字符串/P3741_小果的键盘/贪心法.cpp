#include <iostream>
#include <string>
#include <vector>   // 提供 vector 容器，用来做标记数组
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    string s;
    cin >> n >> s;

    // 标记数组，used[i] = 1 表示 s[i] 已经被某个原有的 "VK" 使用了
    // 使用 vector<char> 而不是 vector<bool> 原因：vector<bool> 是特化版，
    // 其 operator[] 返回的不是真正的 bool 引用，容易引发预期外的错误，
    // 而 vector<char> 行为与普通数组完全一致，更安全直观。
    vector<char> used(n, 0);

    int ans = 0;
    // 第一遍扫描：统计原有的 "VK" 并标记
    for (int i = 0; i + 1 < n; ++i) {
        if (!used[i] && !used[i + 1] && s[i] == 'V' && s[i + 1] == 'K') {
            ++ans;
            used[i] = used[i + 1] = 1;   // 这两个字符被占用
        }
    }

    // 第二遍扫描：查找是否存在未被占用的相邻 "VV" 或 "KK"
    bool canAdd = false;
    for (int i = 0; i + 1 < n; ++i) {
        if (!used[i] && !used[i + 1]) {
            if ((s[i] == 'V' && s[i + 1] == 'V') ||
                (s[i] == 'K' && s[i + 1] == 'K')) {
                canAdd = true;
                break;
            }
        }
    }

    if (canAdd) ++ans;
    cout << ans << '\n';
    return 0;
}