#include <iostream>
#include <string>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string s;
    getline(cin, s);   // 读取整行（含空格）
    int cnt = 0;
    for (char c : s)   // 更简洁的遍历
        if (c != ' ') cnt++;
    cout << cnt << '\n';
    return 0;
}