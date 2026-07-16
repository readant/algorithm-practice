#include <iostream>
using namespace std;

int main() {
    int n, x;
    cin >> n;  // 第一个数是 N
    int cur = 0;  // 0 表示输出 '0'，1 表示输出 '1'
    int cnt = 0;  // 已输出字符数
    while (cin >> x) {
        for (int i = 0; i < x; ++i) {
            cout << (cur ? '1' : '0');
            ++cnt;
            if (cnt % n == 0) cout << '\n';  // 每行满 N 个换行
        }
        cur ^= 1;  // 切换 0/1
    }
    return 0;
}