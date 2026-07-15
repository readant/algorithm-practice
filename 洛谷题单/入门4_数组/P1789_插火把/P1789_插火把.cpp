#include <iostream>
#include <vector>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m, k;
    cin >> n >> m >> k;
    
    // 初始化数组：light标记是否被照亮，item标记是否有物品
    vector<vector<bool>> light(n + 1, vector<bool>(n + 1, false));
    vector<vector<bool>> item(n + 1, vector<bool>(n + 1, false));
    
    // 火把照明范围的相对坐标（共13格）
    int torchDx[] = {-2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
    int torchDy[] = {0, -1, 0, 1, -2, -1, 0, 1, 2, -1, 0, 1, 0};
    int torchCount = 13;
    
    // 萤石照明范围的相对坐标（5x5正方形，共25格）
    vector<int> glowDx, glowDy;
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            glowDx.push_back(i);
            glowDy.push_back(j);
        }
    }
    int glowCount = glowDx.size();
    
    // 处理火把
    for (int i = 0; i < m; i++) {
        int x, y;
        cin >> x >> y;
        item[x][y] = true;
        for (int j = 0; j < torchCount; j++) {
            int nx = x + torchDx[j];
            int ny = y + torchDy[j];
            if (nx >= 1 && nx <= n && ny >= 1 && ny <= n) {
                light[nx][ny] = true;
            }
        }
    }
    
    // 处理萤石
    for (int i = 0; i < k; i++) {
        int x, y;
        cin >> x >> y;
        item[x][y] = true;
        for (int j = 0; j < glowCount; j++) {
            int nx = x + glowDx[j];
            int ny = y + glowDy[j];
            if (nx >= 1 && nx <= n && ny >= 1 && ny <= n) {
                light[nx][ny] = true;
            }
        }
    }
    
    // 统计生成怪物的格子数量
    int monsterCount = 0;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (!light[i][j] && !item[i][j]) {
                monsterCount++;
            }
        }
    }
    
    cout << monsterCount << endl;
    return 0;
}