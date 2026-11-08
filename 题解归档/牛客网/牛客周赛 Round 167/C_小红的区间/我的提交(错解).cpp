// 牛客周赛 Round 167 · C 小红的区间
// 赛事：牛客周赛 Round 167 (2026-10-25)  · 题号：C
// 赛内得分：125 / 150（答案错误，提交于 20:18:20）
// 错在哪：所有分支都写成了 gcd(9,x)==1（复制粘贴错误），没有真正去构造区间 [l,r]
//          使 gcd(区间和, x)=1。能拿 125 应是少量样例恰好命中。这是「样例过就当对了」的复发。
#include <iostream>
#include <algorithm>
using namespace std;
typedef long long ll;

ll gcd(ll a, ll b) {
    while (b) {
        ll t = a % b;
        a = b;
        b = t;
    }
    return a;
}

int main() {
    int x;
    cin >> x;
    if (gcd(9, x) == 1) cout << "2 4\n";
    else if (gcd(9, x) == 1) cout << "1 4\n";
    else if (gcd(9, x) == 1) cout << "1 5\n";
    else cout << "-1\n";
    return 0;
}
