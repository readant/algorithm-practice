#include <iostream>
using namespace std;

int main() {
    int n, x;
    int cnt[11] = {0};
    cin >> n;

    for (int i = 0; i < n; ++i) {
        cin >> x;
        int less = 0;
        for (int v = 0; v < x; ++v)
            less += cnt[v];

        cout << less << " "[i == n-1];
        ++cnt[x];
    }
    return 0;
}