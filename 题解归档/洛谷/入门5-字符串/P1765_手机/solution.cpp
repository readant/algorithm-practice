#include <iostream>   // 提供 cin, cout, getline
#include <string>     // 提供 string 类型
using namespace std;  // 方便使用标准库中的名字（如 cout、string）

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    // 第1步：建立字母到按键次数的映射表
    // 按 a~z 的顺序，每个字母需要按几下
    int press[26] = {
        1,2,3,   // a,b,c
        1,2,3,   // d,e,f
        1,2,3,   // g,h,i
        1,2,3,   // j,k,l
        1,2,3,   // m,n,o
        1,2,3,4, // p,q,r,s  (7键有4个字母)
        1,2,3,   // t,u,v
        1,2,3,4  // w,x,y,z  (9键有4个字母)
    };

    // 第2步：读取整行（包括空格）
    string s;
    getline(cin, s);   // 注意：这里不能用 cin >> s，因为 cin >> 遇到空格就停止

    // 第3步：遍历每个字符并累加
    int ans = 0;
    for (int i = 0; i < s.length(); i++) {
        char ch = s[i];
        if (ch == ' ') {
            ans += 1;          // 空格按1下
        } else {
            // ch 是小写字母，利用 ASCII 码连续的特性，计算下标
            // 'a' 的 ASCII 是 97，所以 'a'-'a'=0，'b'-'a'=1，……
            ans += press[ch - 'a'];
        }
    }

    // 第4步：输出结果
    cout << ans << '\n';
    return 0;
}