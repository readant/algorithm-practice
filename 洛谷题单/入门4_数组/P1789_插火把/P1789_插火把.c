#include <stdio.h>
#include <stdbool.h>

#define MAX_N 102

bool light[MAX_N][MAX_N];
bool item[MAX_N][MAX_N];

// 火把照明范围（相对坐标）
int torch_dx[] = {-2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
int torch_dy[] = {0, -1, 0, 1, -2, -1, 0, 1, 2, -1, 0, 1, 0};
int torch_count = 13;

// 萤石照明范围（相对坐标）
int glow_dx[] = {-2, -2, -2, -2, -2, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2};
int glow_dy[] = {-2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2};
int glow_count = 25;

int main() {
    int n, m, k;
    scanf("%d %d %d", &n, &m, &k);
    
    // 初始化数组
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            light[i][j] = false;
            item[i][j] = false;
        }
    }
    
    // 处理火把
    for (int i = 0; i < m; i++) {
        int x, y;
        scanf("%d %d", &x, &y);
        item[x][y] = true;  // 标记火把位置
        
        // 标记火把照亮的区域
        for (int j = 0; j < torch_count; j++) {
            int nx = x + torch_dx[j];
            int ny = y + torch_dy[j];
            if (nx >= 1 && nx <= n && ny >= 1 && ny <= n) {
                light[nx][ny] = true;
            }
        }
    }
    
    // 处理萤石
    for (int i = 0; i < k; i++) {
        int x, y;
        scanf("%d %d", &x, &y);
        item[x][y] = true;  // 标记萤石位置
        
        // 标记萤石照亮的区域
        for (int j = 0; j < glow_count; j++) {
            int nx = x + glow_dx[j];
            int ny = y + glow_dy[j];
            if (nx >= 1 && nx <= n && ny >= 1 && ny <= n) {
                light[nx][ny] = true;
            }
        }
    }
    
    // 统计生成怪物的格子数量
    int monster_count = 0;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (!light[i][j] && !item[i][j]) {
                monster_count++;
            }
        }
    }
    
    printf("%d\n", monster_count);
    return 0;
}