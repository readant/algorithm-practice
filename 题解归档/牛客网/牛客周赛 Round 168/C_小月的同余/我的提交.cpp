// 牛客周赛 Round 168 · C 小月的同余
// 赛内得分：150 / 150（一次提交 AC，19:55:29，2ms）
// 提交记录：19:55:29 答案正确 150
// 关键点（本场唯一一次读题零失误）：题面后缀写的是 a_i + … + a_n —— 从 a_i 起、与前缀重叠，
//               不是 a_{i+1}。所以条件是 pre[i] ≡ S - pre[i-1] (mod m)，也就是
//               pre[i] + pre[i-1] ≡ S (mod m)，正是下面的写法。
//               （若误读成 pre[i] ≡ S - pre[i]，等价于 2*pre[i] ≡ S，样例 3 3 / 1 2 3 会得 2 而不是 1。）
#include <iostream>
#include <vector>
using namespace std;
typedef long long ll;

ll mod(ll x,ll m)
{
    ll res = x % m;
    if(res < 0) res += m;
    return res;
}

int main() {
    int n;
    ll m;
    int ans = 0;
    cin >> n >> m;
    vector<ll> a(n);
    for(int i = 0;i < n;++i) {
        cin >> a[i];
    }
    vector<ll> pre(n+1,0);

    for(int i = 1;i <= n;++i) {
        pre[i] = pre[i-1] + a[i-1];
    }

    ll sum = pre[n];
    ll t = mod(sum,m);
    for(int i = 1;i <= n;++i) {
        ll t2 = mod(pre[i],m) + mod(pre[i-1],m);
        t2 = mod(t2,m);
        if(t2 == t) {
            ans ++;
        }
    }
    cout << ans << '\n';
    return 0;
}
