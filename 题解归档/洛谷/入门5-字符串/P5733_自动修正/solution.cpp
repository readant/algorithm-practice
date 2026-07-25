#include <iostream>
#include <string>
using namespace std;

int main() {
    ios::sync_with_stdio(false);  // 关闭同步，加速 I/O
    cin.tie(nullptr);
    
    string s;
    cin >> s;
    
    for (int i = 0; i < s.size(); ++i) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            // 位运算：清除第6位，小写转大写
            s[i] &= ~32;
        }
    }
    
    cout << s << '\n';
    return 0;
}
