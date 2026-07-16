#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n, m, k;
    cin >> n >> m >> k;

    vector<vector<bool>> light(n + 1, vector<bool>(n + 1, false));
    vector<vector<bool>> item(n + 1, vector<bool>(n + 1, false));

    // 火把照明范围（13格）
    int torchDx[] = {-2,-1,-1,-1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
    int torchDy[] = { 0,-1, 0, 1,-2,-1, 0, 1, 2,-1, 0, 1, 0};

    for (int i = 0; i < m; i++) {
        int x, y; cin >> x >> y;
        item[x][y] = true;
        for (int j = 0; j < 13; j++) {
            int nx = x + torchDx[j], ny = y + torchDy[j];
            if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
                light[nx][ny] = true;
        }
    }

    for (int i = 0; i < k; i++) {
        int x, y; cin >> x >> y;
        item[x][y] = true;
        for (int dx = -2; dx <= 2; dx++)
            for (int dy = -2; dy <= 2; dy++) {
                int nx = x + dx, ny = y + dy;
                if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
                    light[nx][ny] = true;
            }
    }

    int ans = 0;
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= n; j++)
            if (!light[i][j] && !item[i][j])
                ans++;

    cout << ans << endl;
    return 0;
}
