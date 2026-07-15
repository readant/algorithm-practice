#include <iostream>
#include <iomanip>

using namespace std;

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);

    int n;
    cin >> n;
    
    // n 最大 9 ，多开一个防止越界
    int a[10][10] = {0}; 
     
    // 方向：右、下、左、上
    // 这是直接预设好的旋转方向吗

    int dx[4] = {0,1,0,-1};
    int dy[4] = {1,0,-1,0};

    int x = 0,y = 0;          // 当前位置  
    int dir = 0;              // 初始方向始终向右

    for(int num = 1;num <= n * n;++num){
        a[x][y] = num;

        // 计算下一步位置
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        // 如果下一步越界或已填过，则转向
        if(nx < 0 || nx >= n || ny <0 || ny >=n || a[nx][ny] != 0){
            dir = (dir + 1) % 4;
            nx = x + dx[dir];
            ny = y +dy[dir];
        }

        x = nx;
        y = ny;
    }

    return 0;
}