#include <stdio.h>
#include <string.h>

int main() {
    int n;
    scanf("%d", &n);

    int a[10][10];
    memset(a, 0, sizeof(a));

    // 方向：右、下、左、上
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};

    int x = 0, y = 0, dir = 0;

    for (int num = 1; num <= n * n; ++num) {
        a[x][y] = num;
        int nx = x + dx[dir];
        int ny = y + dy[dir];
        // 越界或已填充则转向
        if (nx < 0 || nx >= n || ny < 0 || ny >= n || a[nx][ny] != 0) {
            dir = (dir + 1) % 4;
            nx = x + dx[dir];
            ny = y + dy[dir];
        }
        x = nx;
        y = ny;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%3d", a[i][j]);
        }
        printf("\n");
    }
    return 0;
}
