// 牛客周赛 Round 166 · C 题 · 小月的长廊（正解）
// 复杂度：O(n log n)
//
// 思路：
//   d(x) 只在端点处发生变化，把 2n 个端点离散化后做差分，一次前缀和求出每个端点的覆盖数。
//   再逐区间比较 |d(l_i) - d(r_i)| 与 p。
//   好处是不用真的去维护数轴长度（坐标可达 1e9）。

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n;
    long long p;
    cin >> n >> p;

    vector<long long> l(n), r(n), xs;
    xs.reserve(2 * n);
    for (int i = 0; i < n; i++) {
        cin >> l[i] >> r[i];
        xs.push_back(l[i]);
        xs.push_back(r[i]);
    }

    sort(xs.begin(), xs.end());
    xs.erase(unique(xs.begin(), xs.end()), xs.end());
    int m = (int)xs.size();

    vector<long long> diff(m + 1, 0);
    for (int i = 0; i < n; i++) {
        int a = (int)(lower_bound(xs.begin(), xs.end(), l[i]) - xs.begin());
        int b = (int)(lower_bound(xs.begin(), xs.end(), r[i]) - xs.begin());
        diff[a] += 1;
        diff[b + 1] -= 1;
    }

    vector<long long> d(m, 0);
    long long cur = 0;
    for (int i = 0; i < m; i++) {
        cur += diff[i];
        d[i] = cur;
    }

    long long cnt = 0;
    for (int i = 0; i < n; i++) {
        int a = (int)(lower_bound(xs.begin(), xs.end(), l[i]) - xs.begin());
        int b = (int)(lower_bound(xs.begin(), xs.end(), r[i]) - xs.begin());
        long long df = d[a] - d[b];
        if (df < 0) df = -df;
        if (df >= p) cnt++;
    }

    cout << cnt << "\n";
    return 0;
}
