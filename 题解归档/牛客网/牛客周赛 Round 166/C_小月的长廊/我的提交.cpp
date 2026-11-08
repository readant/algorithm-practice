// 牛客周赛 Round 166 · C 题 · 小月的长廊
// 赛内得分：26.25 / 150（提交于 00:19:42）
// 说明：本人赛内提交代码，原样归档，未做任何修改。
//
// 错在哪：把 d(x) 理解成了「区间 [l_i, r_i] 的长度」，而题目定义是
//        「覆盖位置 x 的结界数量」。于是判据写成了 r - l + 1 > p，
//        与要求的 |d(l_i) - d(r_i)| >= p 完全无关。
// 为什么没被发现：样例 1（n=5, p=2）算出来恰好也是 4，样例过了就交了。
// 正解见同目录 C_小月的长廊.cpp（扫描线求覆盖数）。

#include <iostream>

using namespace std;

int main() {
    int n,p;
    cin >> n >> p;
    int cnt = 0;
    while(n--) {
        int l,r;
        cin >> l >> r;
        if(r - l + 1 > p) cnt ++;
    }
    cout << cnt << endl;
    return 0;
}
