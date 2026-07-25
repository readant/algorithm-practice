#include <iostream>
using namespace std;

const int MAXM = 2000005;
bool light[MAXM];

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    int n;
    cin >> n;
    for (int i = 0; i < n; ++i) {
        double a;
        int t;
        cin >> a >> t;
        long long A = (long long)(a * 1000000 + 0.5);
        for (int j = 1; j <= t; ++j) {
            long long idx = (j * A) / 1000000;
            light[idx] = !light[idx];
        }
    }
    
    for (int i = 1; ; ++i) {
        if (light[i]) {
            cout << i << '\n';
            break;
        }
    }
    return 0;
}