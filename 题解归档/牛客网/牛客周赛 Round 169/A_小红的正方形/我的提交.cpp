// 牛客周赛 Round 169 · A 小红的正方形
// 赛内得分：50 / 50（答案正确，第 1 次提交，19:08:15）
// 赛内写法原样保留，未做任何修改。
#include <bits/stdc++.h>

using namespace std;
typedef long long ll;
int main() {
    ll x,y;
    ll maxx = -1e18,minx = 1e18;
    for(int i = 0;i<4;i++) {
        cin >> x >> y;
        minx = min(minx,x);
        maxx = max(maxx,x);
    }
    cout << maxx - minx << endl;
    return 0;
}
