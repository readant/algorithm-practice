// 牛客周赛 Round 167 · D 小红的排列构造
// 赛事：牛客周赛 Round 167 (2026-10-25)  · 题号：D
// 赛内得分：截图提交页显示 20.67（答案错误）；封榜最终 D = 26.67（最终提交代码未抓取，此为中间提交）。
// 错在哪：逻辑混乱——外层 for 没正常闭合、内层又用同名变量 i 重开循环；没有真正按「三组奇偶性相同」
//          去构造排列。用了 <bits/stdc++.h>（与题解规范不符，但此处是本人赛场原样留档，不作修改）。
#include <bits/stdc++.h>

using namespace std;

int main() {
    int ca = 0, cb = 0, cc = 0;
    int n;
    string s;
    cin >> n >> s;
    int p[200005];
    for (int i = 0; i < n; i++) {
        p[i] = i + 1;

        for (int i = 0; i < n; i++) {
            if (s[i] == 'a') ca++;
            else if (s[i] == 'b') cb++;
            else cc++;
        }

        int ra = ca % 2, rb = cb % 2, rc = cc % 2;
        if (ra == rb && rb == rc) {
            for (int i = 0; i < n; i++) cout << p[i] << " ";
            return 0;
        }
        int pa = -1, pb = -1;
        for (int i = 0; i < n; i++) {
            if (s[i] == 'a' && pa == -1) pa = i;
            if (s[i] == 'b' && pb == -1) pb = i;
        }
        if (pa != -1 && pb != -1) {
            swap(p[pa], p[pb]);
            for (int i = 0; i < n; i++) cout << p[i] << " ";
            return 0;
        }
        cout << -1;
        return 0;
    }
}
