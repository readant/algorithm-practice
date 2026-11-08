// 牛客周赛 Round 166 · A 题 · 小月的增减
// 赛内得分：50 / 50（AC，提交于 00:04:15）
// 说明：本人赛内提交代码，原样归档，未做任何修改。

#include <iostream>

using namespace std;

int main() {
    int n,x = 0;
    cin >> n >> x;
    int a[101];
    for(int i = 0;i<n;i++) {
        cin >> a[i];
        if(a[i] % 2 == 0) x+=1;
        else x-=1;
    }
    cout << x << endl;
    return 0;
}
