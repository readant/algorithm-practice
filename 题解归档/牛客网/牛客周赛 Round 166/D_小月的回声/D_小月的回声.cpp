// 牛客周赛 Round 166 · D 题 · 小月的回声（正解）
// 复杂度：O(900 n)（常数由 a_i 上界 30 决定）
//
// 关键一步：a_i >= 1，所以中间和 sum >= r - l - 1；
//           而 a_l * a_r <= 30 * 30 = 900。
//           于是只要 sum > 900 就不可能再命中，直接 break。
//           每个左端点最多向右扫 901 个位置。

#include <iostream>
using namespace std;

int a[200005];

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n;
    cin >> n;
    for (int i = 1; i <= n; i++) cin >> a[i];

    long long cnt = 0;
    for (int l = 1; l <= n; l++) {
        long long s = 0;
        for (int r = l + 2; r <= n; r++) {
            s += a[r - 1];
            if (s > 900) break;
            if (s == (long long)a[l] * a[r]) cnt++;
        }
    }

    cout << cnt << "\n";
    return 0;
}
