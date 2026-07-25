# 搜索算法（DFS & BFS）

## 概述

搜索算法用于遍历图或树的结构。DFS 和 BFS 是两种最基本的搜索策略。

---

## 对比总览

| 特性 | DFS（深度优先） | BFS（广度优先） |
|------|----------------|----------------|
| 数据结构 | 栈（递归调用栈） | 队列 |
| 搜索方向 | 一条路走到底 | 逐层扩展 |
| 空间复杂度 | O(h) h为深度 | O(w) w为最大宽度 |
| 适用场景 | 路径问题、回溯 | 最短路径、层序遍历 |

---

## 1. DFS（深度优先搜索）

**核心思想**：一条路走到底，走不通再回头

### 模板：图的DFS

```cpp
#include <iostream>
#include <vector>
using namespace std;

const int MAXN = 100;
vector<int> adj[MAXN];
bool visited[MAXN];

void dfs(int u) {
    visited[u] = true;
    cout << u << " ";
    
    for (int v : adj[u]) {
        if (!visited[v]) {
            dfs(v);
        }
    }
}

int main() {
    // 建图
    adj[0] = {1, 2};
    adj[1] = {3};
    adj[2] = {4};
    
    dfs(0);  // 输出: 0 1 3 2 4
    
    return 0;
}
```

### 模板：网格DFS（回溯）

```cpp
int dx[] = {0, 0, 1, -1};
int dy[] = {1, -1, 0, 0};

void dfs(vector<vector<int>>& grid, int x, int y) {
    int n = grid.size(), m = grid[0].size();
    
    // 边界检查 + 访问检查
    if (x < 0 || x >= n || y < 0 || y >= m || grid[x][y] == 0)
        return;
    
    grid[x][y] = 0;  // 标记已访问
    
    for (int i = 0; i < 4; i++) {
        dfs(grid, x + dx[i], y + dy[i]);
    }
}
```

---

## 2. BFS（广度优先搜索）

**核心思想**：逐层扩展，先访问距离近的节点

### 模板：图的BFS

```cpp
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

const int MAXN = 100;
vector<int> adj[MAXN];
bool visited[MAXN];

void bfs(int start) {
    queue<int> q;
    q.push(start);
    visited[start] = true;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        cout << u << " ";
        
        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
}

int main() {
    adj[0] = {1, 2};
    adj[1] = {3};
    adj[2] = {4};
    
    bfs(0);  // 输出: 0 1 2 3 4
    
    return 0;
}
```

### 模板：网格BFS（最短路径）

```cpp
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

int dx[] = {0, 0, 1, -1};
int dy[] = {1, -1, 0, 0};

int bfs(vector<vector<int>>& grid, int sx, int sy, int tx, int ty) {
    int n = grid.size(), m = grid[0].size();
    
    if (grid[sx][sy] == 0 || grid[tx][ty] == 0) return -1;
    
    queue<pair<int, int>> q;
    q.push({sx, sy});
    grid[sx][sy] = 0;  // 标记已访问
    
    int dist = 0;
    
    while (!q.empty()) {
        int size = q.size();
        
        for (int i = 0; i < size; i++) {
            auto [x, y] = q.front();
            q.pop();
            
            if (x == tx && y == ty) return dist;
            
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d], ny = y + dy[d];
                
                if (nx >= 0 && nx < n && ny >= 0 && ny < m && grid[nx][ny] == 1) {
                    grid[nx][ny] = 0;
                    q.push({nx, ny});
                }
            }
        }
        dist++;
    }
    
    return -1;
}
```

---

## 3. 经典应用

### 岛屿数量（DFS）

```cpp
int numIslands(vector<vector<char>>& grid) {
    int count = 0;
    int n = grid.size(), m = grid[0].size();
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (grid[i][j] == '1') {
                dfs(grid, i, j);
                count++;
            }
        }
    }
    return count;
}
```

### 课程表（BFS 拓扑排序）

```cpp
bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    vector<int> inDegree(numCourses, 0);
    
    for (auto& pre : prerequisites) {
        adj[pre[1]].push_back(pre[0]);
        inDegree[pre[0]]++;
    }
    
    queue<int> q;
    for (int i = 0; i < numCourses; i++) {
        if (inDegree[i] == 0) q.push(i);
    }
    
    int count = 0;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        count++;
        
        for (int v : adj[u]) {
            if (--inDegree[v] == 0) {
                q.push(v);
            }
        }
    }
    
    return count == numCourses;
}
```

---

## 如何选择

```
求最短路径？        → BFS
求所有路径？        → DFS
树的层序遍历？      → BFS
图的连通分量？      → DFS
拓扑排序？          → BFS（Kahn算法）
回溯问题？          → DFS
```

---

> 相关内容：[参考层/01-C++/01-STL容器速查.md](../../../01-参考层/01-C++/01-STL容器速查.md) — queue/stack 用法
