// 牛客周赛 Round 165 · D 小月的删数   （正解 / O(n)）
//
// 题意：选一个在数组中出现过的值 c，删除所有等于 c 的元素，其余保持相对顺序，
//       得到 b（长度 m）。读数 = b1 - b2 + b3 - ...（奇数位和 - 偶数位和）m=0 时为 0。
//       求所有 c 中 |读数| 的最小值。
//
// 关键：删除元素会改变后续元素在新数组中的“位置奇偶性”，所以读数不是“按原位置符号挑元素”，
//       而是“按新位置逐元素交替求和”。等价于：扫描剩余序列，符号从 + 开始逐个翻转。
//
// 按 c 的出现位置把数组切成若干“保留段”[L,R]（两段之间是被删掉的 c）。每段内部连续，
// 于是该段对读数的贡献 = （该段起始符号）×（该段以 + 打头的交替和）：
//   段内交替和  S = sum_{j=L..R} (-1)^{j-L} a[j] = (-1)^L * (pref[R+1]-pref[L])
//     其中 pref[i] = a0 - a1 + a2 - ... ± a_{i-1}（按原下标交替的“前缀交替和”）
//   起始符号由 L 的新位置 rank=(L+1)-t 决定（t = L 之前被删的 c 的个数）：
//     rank 为奇数 -> +，偶数 -> -。
//
// 每个 c 只需处理 O(出现次数+1) 段，所有 c 合计 O(n log n)（分组用排序/哈希）。
//
// 本写法用 unordered_map 分组；若担心被卡哈希可改为排序+离散化。

#include <cstdio>          // scanf / printf
#include <vector>          // vector
#include <unordered_map>   // unordered_map
#include <algorithm>       // min
#include <climits>         // LLONG_MAX
#include <cstdlib>         // llabs
using namespace std;

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    vector<long long> a(n);
    for (int i = 0; i < n; i++) scanf("%lld", &a[i]);

    // pref[i] = a[0]-a[1]+...±a[i-1]
    vector<long long> pref(n + 1, 0);
    for (int i = 0; i < n; i++)
        pref[i + 1] = pref[i] + ((i % 2 == 0) ? a[i] : -a[i]);

    unordered_map<long long, vector<int>> pos;
    pos.reserve((size_t)n * 2 + 10);
    for (int i = 0; i < n; i++) pos[a[i]].push_back(i);

    long long best = LLONG_MAX;
    for (auto& kv : pos) {
        const vector<int>& v = kv.second;
        long long reading = 0;
        int sz = (int)v.size();
        for (int t = 0; t <= sz; t++) {
            int L = (t == 0) ? 0 : (v[t - 1] + 1);          // 段起点
            int R = (t == sz) ? (n - 1) : (v[t] - 1);       // 段终点
            if (L > R) continue;
            long long D = pref[R + 1] - pref[L];            // sum (-1)^j a[j]
            long long S = (L % 2 == 0) ? D : -D;            // 段内以 + 打头的交替和
            long long rank = (long long)(L + 1) - t;        // L 在新数组中的位置（1-based）
            long long sign = (rank % 2 == 1) ? 1 : -1;
            reading += sign * S;
        }
        best = min(best, llabs(reading));
    }
    printf("%lld\n", best);
    return 0;
}
