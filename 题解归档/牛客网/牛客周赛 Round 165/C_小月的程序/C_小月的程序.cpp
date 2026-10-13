// 牛客周赛 Round 165 · C 小月的程序   （正解 / O(n)）
//
// 题意：每个测试给长度 n 的数组 a。可选一个 k(1<=k<=n)，把前 k 个元素原地翻转，
//       翻转后记为 b，要求 b1<=b2<=...<=bn（非递减）。统计合格的 k 的个数。
//
// 充要条件（a 为 0-indexed，翻转前缀长度 k 后）：
//   ① 反转段本身非递减：a[k-1] <= a[k-2] <= ... <= a[0]  <=>  a[0] >= a[1] >= ... >= a[k-1]
//      （前 k 个在原序下"非增"）
//   ② 未反转的后缀非递减：a[k] <= a[k+1] <= ... <= a[n-1]
//   ③ 交界处：a[0] <= a[k]（k<n 时需要；k=n 时无交界）
//
// 预处理两条 O(n) 信息即可扫一遍计数：
//   L1 = 最长的"非增前缀"长度（满足 a[0]>=...>=a[L1-1]，取最大）
//   R0 = 最短的"非减后缀"起点（满足 a[R0]<=...<=a[n-1]，取最小）
// 于是 k 合法 <=> k<=L1 且 (k==n 时 L1==n) 或 (k>=R0 且 a[0]<=a[k])。
//
// 复杂度 O(n)（sum n <= 2e5）。

#include <cstdio>      // scanf / printf
#include <vector>      // vector
#include <algorithm>   // max / min
using namespace std;

int main() {
    int T;
    if (scanf("%d", &T) != 1) return 0;
    while (T--) {
        int n;
        scanf("%d", &n);
        vector<long long> a(n);
        for (int i = 0; i < n; i++) scanf("%lld", &a[i]);

        // L1：最长非增前缀长度（至少 1）
        int L1 = 1;
        while (L1 < n && a[L1 - 1] >= a[L1]) L1++;

        // R0：最短非减后缀的起点（0-based）
        int R0 = n - 1;
        while (R0 > 0 && a[R0 - 1] <= a[R0]) R0--;

        long long cnt = 0;
        for (int k = 1; k <= n; k++) {
            if (k > L1) break;                 // ① 前 k 个必须非增
            if (k == n) {                      // 整体翻转：需整个数组非增
                if (L1 == n) cnt++;
                continue;
            }
            if (k < R0) continue;              // ② 后缀 a[k..] 必须非减
            if (a[0] <= a[k]) cnt++;           // ③ 交界
        }
        printf("%lld\n", cnt);
    }
    return 0;
}
