#include <stdio.h>
#include <string.h>

int main() {
    int n, m, k;
    scanf("%d%d%d", &n, &m, &k);

    int light[102][102] = {0};
    int item[102][102] = {0};

    int torchDx[] = {-2,-1,-1,-1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
    int torchDy[] = { 0,-1, 0, 1,-2,-1, 0, 1, 2,-1, 0, 1, 0};

    for (int i = 0; i < m; i++) {
        int x, y; scanf("%d%d", &x, &y);
        item[x][y] = 1;
        for (int j = 0; j < 13; j++) {
            int nx = x + torchDx[j], ny = y + torchDy[j];
            if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
                light[nx][ny] = 1;
        }
    }

    for (int i = 0; i < k; i++) {
        int x, y; scanf("%d%d", &x, &y);
        item[x][y] = 1;
        for (int dx = -2; dx <= 2; dx++)
            for (int dy = -2; dy <= 2; dy++) {
                int nx = x + dx, ny = y + dy;
                if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
                    light[nx][ny] = 1;
            }
    }

    int ans = 0;
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= n; j++)
            if (!light[i][j] && !item[i][j])
                ans++;

    printf("%d\n", ans);
    return 0;
}
