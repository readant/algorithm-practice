// 牛客周赛 Round 167 · B qcjj的演唱会
// 赛事：牛客周赛 Round 167 (2026-10-25)  · 题号：B
// 赛内得分：100 / 100（AC，提交于 20:03:32）
// 错在哪：无，一次过。思路：三场各排序，对第一场每个歌二分查是否在二、三场都出现。
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

string s[3][100001];

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >> n;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < n; j++) {
            cin >> s[i][j];
        }
    }
    sort(s[1], s[1] + n);
    sort(s[2], s[2] + n);
    vector<string> ans;
    for (int i = 0; i < n; i++) {
        string now = s[0][i];
        bool ok1 = binary_search(s[1], s[1] + n, now);
        bool ok2 = binary_search(s[2], s[2] + n, now);
        if (ok1 && ok2) {
            ans.push_back(now);
        }
    }
    cout << ans.size() << '\n';
    if (!ans.empty()) {
        for (size_t i = 0; i < ans.size(); i++) {
            if (i > 0) cout << ' ';
            cout << ans[i];
        }
        cout << '\n';
    }
    return 0;
}
