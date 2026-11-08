// 牛客周赛 Round 168 · D 小月的储物格
// 赛内得分：33.33 / 200（未通过）
// 提交记录：20:28:32 运行超时 16.67 / 20:30:34 运行超时 25 / 20:31:48 运行超时 33.33
// 当时错在哪：思路与样例都对（样例 5 3 / 10 20 30 40 50 / 0 0 4 输出 60 3，与题面说明一致），
//               但把「走 d_i 步」真的循环走了 d_i 次，而 d_i ≤ 10^9、m ≤ 2×10^5，
//               总步数上界是 Σd_i ≈ 2×10^14；即使只单轮 d = 10^9，本机也要 0.43 秒。
//               正确方向是「跳过已占用」的秩查询（并查集 next 指针 / 树状数组上二分找第 k 个空位），
//               并且先把 d_i 对剩余空位数取模。
#include <iostream>
#include <vector>
using namespace std;
typedef long long ll;

int main() {
    int n,m;
    cin >> n >> m;
    vector<ll> a(n+1);
    for(int i = 1;i <= n;++i) {
        cin >> a[i];
    }
    vector<ll> d(m);
    for(int i = 0;i < m;++i) {
        cin >> d[i];
    }
    vector<bool> vis(n + 1,false);

    ll sum = 0;
    int cur = 1;
    int last = 0;
    for(ll i = 0;i < m;++i) {
        ll step = d[i];
        int p = cur;
        for(ll j = 0;j < step;++j) {
            if(p==n) p = 1;
            else p = p + 1;
        }

        while(vis[p]) {
            if(p==n) p = 1;
            else p = p + 1;
        }
        vis[p] = true;
        sum += a[p];
        last = p;
        cur = p;
    }
    cout << sum << ' ' << last << '\n';
    return 0;
}
