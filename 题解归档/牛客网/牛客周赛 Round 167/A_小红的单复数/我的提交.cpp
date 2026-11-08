// 牛客周赛 Round 167 · A 小红的单复数
// 赛事：牛客周赛 Round 167 (2026-10-25)  · 题号：A
// 赛内得分：50 / 50（AC，提交于 19:07:59）
// 错在哪：无，一次过。注意 len(s)>=3，取末两位不会越界。
#include <iostream>
#include <string>
using namespace std;

int main() {
    string s;
    cin >> s;
    if (s.substr(s.size() - 2, 2) == "es") {
        s = s.substr(0, s.size() - 2);
    } else if (s.back() == 's') {
        s.pop_back();
    }
    cout << s << endl;
    return 0;
}
