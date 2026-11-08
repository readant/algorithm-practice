// 牛客周赛 Round 169 · C 小红的灯
// 赛内得分：150 / 150（答案正确，第 1 次提交，20:02:49，4ms）
// 做法：i 只含质因子 2、3 时，切换所有 i 的倍数；最后数开着的灯。
//       bool a[] 下标从 1 开始，a[0] 空置。
// 赛内写法原样保留，未做任何修改。
#include <bits/stdc++.h>

using namespace std;
bool a[200001] = {false};

bool is_(int x) {
    while(x % 2 == 0) x/=2;
    while(x % 3 == 0) x/=3;
    return x == 1;
}

int main() {
    int n;
    cin >> n;

    for(int i = 1;i <= n;i++) {
        if(is_(i)) {
            for(int j = i;j<=n;j += i) {
                a[j] = !a[j];
            }
        }
    }

    int ans = 0;
    for(int i = 1;i<=n;i++) {
        if(a[i]) ans++;
    }
    cout << ans << '\n';

    return 0;
}
