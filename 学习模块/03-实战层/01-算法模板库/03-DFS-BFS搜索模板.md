# 搜索算法模板

## DFS（深度优先搜索）

**原理**：沿着一条路径尽可能深地搜索，无法继续时回溯。用栈（或递归）实现。

**时间复杂度**：O(V + E)，V 为顶点数，E 为边数
**空间复杂度**：O(h)，h 为树高

```cpp
int dx[] = {0, 0, 1, -1};
int dy[] = {1, -1, 0, 0};

void dfs(int x, int y) {
    if (边界条件) return;
    vis[x][y] = true;
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (边界检查通过 && !vis[nx][ny]) dfs(nx, ny);
    }
}
```

**非递归版（显式栈）**：

```cpp
void dfs(int startX, int startY) {
    stack<pair<int,int>> st;
    st.push({startX, startY});
    vis[startX][startY] = true;
    
    while (!st.empty()) {
        auto [x, y] = st.top(); st.pop();
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i], ny = y + dy[i];
            if (边界检查通过 && !vis[nx][ny]) {
                vis[nx][ny] = true;
                st.push({nx, ny});
            }
        }
    }
}
```

---

## BFS（广度优先搜索）

**原理**：逐层搜索，先访问所有距离为 1 的节点，再访问距离为 2 的节点。用队列实现。

**时间复杂度**：O(V + E)
**空间复杂度**：O(w)，w 为图的宽度
**特点**：无权图的最短路径

```cpp
void bfs(int startX, int startY) {
    queue<pair<int,int>> q;
    q.push({startX, startY});
    vis[startX][startY] = true;
    
    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i], ny = y + dy[i];
            if (边界检查通过 && !vis[nx][ny]) {
                vis[nx][ny] = true;
                q.push({nx, ny});
            }
        }
    }
}
```

**带距离的 BFS**：

```cpp
void bfs(int startX, int startY) {
    queue<pair<int,int>> q;
    q.push({startX, startY});
    dist[startX][startY] = 0;
    vis[startX][startY] = true;
    
    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i], ny = y + dy[i];
            if (边界检查通过 && !vis[nx][ny]) {
                vis[nx][ny] = true;
                dist[nx][ny] = dist[x][y] + 1;
                q.push({nx, ny});
            }
        }
    }
}
```

---

## 二分搜索

**原理**：在有序数组中，每次将搜索范围缩小一半。

**时间复杂度**：O(log n)

```cpp
int binarySearch(vector<int>& arr, int target) {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}
```

**找左边界**：

```cpp
int lowerBound(vector<int>& arr, int target) {
    int left = 0, right = arr.size() - 1, ans = -1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] >= target) {
            ans = mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return ans;
}
```

**找右边界**：

```cpp
int upperBound(vector<int>& arr, int target) {
    int left = 0, right = arr.size() - 1, ans = -1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] <= target) {
            ans = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return ans;
}
```

**二分答案**：

```cpp
int binaryAnswer(int left, int right, function<bool(int)> check) {
    int ans = 0;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (check(mid)) {
            ans = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return ans;
}
```

---

## DFS vs BFS 对比

| 特性 | DFS | BFS |
|------|-----|-----|
| 数据结构 | 栈/递归 | 队列 |
| 空间 | O(h) 树高 | O(w) 最宽 |
| 最短路径 | 不保证 | 保证（无权图） |
| 适用场景 | 连通性、排列组合、回溯 | 最短路径、层序遍历 |
