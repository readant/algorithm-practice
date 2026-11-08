// 前缀和版本，用于前缀和练习


#include <iostream>
#include <algorithm>
using namespace std;
typedef long long ll;

const int MAXN = 2e5 + 10;
int a[MAXN];
ll s[MAXN];
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    int a[100001];
    for(int i = 0;i<n;i++) {
        cin >> a[i]; 
        // 前缀和公式 s[i] = 前i项总和
        s[i] = s[i - 1] + a[i]; 
    } 
    // min_s 记录 r 之前，最小的前缀和，初始s[0] = 0    
    ll min_s = s[0];
    // 答案初始设极小负数，应对全是负数的情况
    ll ans = -1e18;

    // 枚举区间右端点 r
    for(int r = 1;r <= n;r++) {
        // 区间[l,r] 和 = s[r] - s[l-1]
        // 固定,减去前面最小的s，就能得到以r结尾的最大子段和
        ans = max(ans,s[r] - min_s);

        // 更新最小前缀和：把当前s[r] 纳入候选，给后面的r 使用
        min_s = min(min_s,s[r]);

    }
    cout << ans << endl;
    return 0;
}