// 牛客周赛 Round 168 · B 小月的屏幕
// 赛内得分：100 / 100（第 4 次提交 AC，19:29:13）
// 提交记录：19:22:20 答案错误 85 / 19:23:41 答案错误 85 / 19:25:38 答案错误 85 / 19:29:13 答案正确 100
// 当时错在哪：前三次是「加特判」的方向（代码 659–704B），反而在边界上出错；
//               最终 AC 版把逻辑收回成一条 i/j 边界判断（502B），减法做对。
//               本题 n,m ≤ 2000 且 n*m ≤ 4×10^5，逐格判断外圈即可。
#include <iostream>
#include <vector>
#include <string>
using namespace std;

int main()
{

    int n, m;
    ios::sync_with_stdio(0);
    cin.tie(0);
    cin >> n >> m;
    vector<string> s(n);
    for (int i = 0; i < n; ++i)
    {
        cin >> s[i];
    }
    int ans = 0;
    for(int i = 0;i<n;++i) {
        for(int j = 0;j < m;++j ){
            if(i == 0||j==0||i==n-1||j==m-1 ){
                if(s[i][j] == '#') ans ++;
            }
        }
    }
    cout << ans << '\n';
    return 0;
}
