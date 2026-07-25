#include <iostream>
using namespace std;

int main() {
    int n, m, q;
    cin >> n >> m >> q;
    
    int grid[55][55] = {0};
    
    while (q--) {
        int x1, y1, x2, y2;
        cin >> x1 >> y1 >> x2 >> y2;
        
        for (int i = x1; i <= x2; ++i) {
            for (int j = y1; j <= y2; ++j) {
                grid[i][j] = 1;
            }
        }
    }
    
    int cnt = 0;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (grid[i][j] == 0) {
                cnt++;
            }
        }
    }

    cout << cnt << endl;
    return 0;
}