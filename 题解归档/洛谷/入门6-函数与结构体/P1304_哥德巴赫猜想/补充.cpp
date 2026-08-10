#include <iostream>   // cin, cout
#include <cmath>      // sqrt 函数

using namespace std;

// 判断 n 是否为质数的函数
bool isPrimeNum(int n) {
    if (n < 2) return false;
    // 只需要检查到 sqrt(n) 即可
    int limit = (int)sqrt(n);  // cmath 中的 sqrt 返回 double，强转 int
    for (int i = 2; i <= limit; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;

    for (int x = 4; x <= N; x += 2) {
        for (int p = 2; p <= x / 2; ++p) {
            if (isPrimeNum(p) && isPrimeNum(x - p)) {
                cout << x << '=' << p << '+' << (x - p) << '\n';
                break;
            }
        }
    }

    return 0;
}