// 牛客周赛 Round 169 · D 小红跑步（错解：方向判定错误）
// 赛内得分：186.67 / 200（答案错误，全场唯一一次 D 提交，20:59:30 —— 距比赛结束 30 秒）
// 测试点：16 个点中 #1、#3 答案错误，其余全过（即 93.3%）
//
// 错在哪：遍历用固定方向优先级 dx={-1,1,0,0} / dy={0,0,-1,1}（上、下、左、右）挑下一格，
//         第一步的走向完全由这个优先级决定。但环路方向是全局概念，走出来的圈是顺时针
//         还是逆时针，取决于起点在环上的位置和环的形状，不由优先级保证。
//         反例：2x3 矩形边框，从左上角出发「上」越界而选「下」，得到逆时针；
//               同样代码从右上角出发，先选「下」恰好是顺时针。
//         于是答案有时是 dis、有时应是 len-dis。
// 赛内写法原样转录自牛客提交页，仅缩进可能有出入。
#include <bits/stdc++.h>

using namespace std;

// 方向 上下左右
int dx[] = {-1,1,0,0};
int dy[] = {0,0,-1,1};

int main() {
    int n,m;
    cin >> n >> m;
    vector<string> s(n);
    for(int i = 0;i < n;i++) {
        cin >> s[i];
    }

    int x1,y1,x2,y2;
    cin >> x1 >> y1 >> x2 >> y2;
    x1--;y1--;
    x2--;y2--;
    // 记录
    vector<pair<int,int>> path;
    // 现在
    int cx = x1,cy = y1;
    // 来路
    int px = -1,py = -1;

    while(1) {
        path.emplace_back(cx,cy);

        int nx = -1,ny = -1;

        for(int d = 0;d < 4;d++) {
            int tx = cx + dx[d];
            int ty = cy + dy[d];
            if(tx < 0 || tx >= n || ty < 0 || ty >= m) continue;
            if(s[tx][ty] != '*') continue;
            if(tx == px && ty == py) continue;
            nx = tx;
            ny = ty;
            break;
        }
        px = cx;
        py = cy;
        cx = nx;
        cy = ny;
        if(cx == x1 && cy == y1) break;
    }

    int len = path.size();
    int s_idx = -1,t_idx = -1;

    for(int i = 0;i < len;i++) {
        int x = path[i].first;
        int y = path[i].second;
        if(x == x1 && y == y1) s_idx = i;
        if(x == x2 && y == y2) t_idx = i;
    }

    int dis = (t_idx - s_idx + len) % len;
    cout << dis << '\n';

    return 0;
}
