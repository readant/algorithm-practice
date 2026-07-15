# P5731 【深基5.习6】蛇形方阵 - 题解

## 题目分析

### 问题理解

给定一个正整数 n（1≤n≤9），要求输出 n×n 的蛇形方阵：

- 从左上角开始，按顺时针方向螺旋填充数字 1 到 n²
- 每个数字占3个字符宽度，右对齐，不足补空格

### 核心难点

1. **螺旋顺序模拟**：需要精确控制填充方向
2. **边界检测**：在矩阵边界处正确转向
3. **格式化输出**：满足严格的宽度要求

## 算法思路

### 方法：方向模拟法

1. **方向数组**：定义四个方向（右、下、左、上）
2. **当前位置**：维护 (x, y) 坐标和当前方向
3. **填充逻辑**：
   - 填充当前格子
   - 计算下一步位置
   - 检查是否越界或已填充，需要转向
   - 更新位置和方向
4. **重复**：直到填满所有 n² 个格子

### 方向转换

使用模运算实现顺时针旋转：

```cpp
dir = (dir + 1) % 4;  // 0→1→2→3→0
```

## 代码实现

### 版本1：使用 `<iomanip>` 格式化输出

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n;
    cin >> n;
    int a[10][10] = {0};
    
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    int x = 0, y = 0, dir = 0;
    
    for (int num = 1; num <= n * n; ++num) {
        a[x][y] = num;
        int nx = x + dx[dir];
        int ny = y + dy[dir];
        
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
            cout << setw(3) << a[i][j];
        }
        cout << '\n';
    }
    return 0;
}
```

### 版本2：纯 `<iostream>` 底层实现

```cpp
#include <iostream>
using namespace std;

void printNum(int num, int width) {
    if (num == 0) {
        for (int i = 0; i < width - 1; ++i) cout << ' ';
        cout << 0;
        return;
    }
    int temp = num, digits = 0;
    while (temp > 0) {
        digits++;
        temp /= 10;
    }
    for (int i = 0; i < width - digits; ++i) cout << ' ';
    cout << num;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n;
    cin >> n;
    int a[10][10] = {0};
    
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    int x = 0, y = 0, dir = 0;
    
    for (int num = 1; num <= n * n; ++num) {
        a[x][y] = num;
        int nx = x + dx[dir];
        int ny = y + dy[dir];
        
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
            printNum(a[i][j], 3);
        }
        cout << '\n';
    }
    return 0;
}
```

## 复杂度分析

- **时间复杂度**：O(n²)
- **空间复杂度**：O(n²)（存储矩阵）

## 测试验证

### 样例测试

输入：

```
4
```

输出：

```
  1  2  3  4
 12 13 14  5
 11 16 15  6
 10  9  8  7
```

### 边界测试

1. **n=1**：输出单个数字 `1`
2. **n=9**：测试最大规模，验证边界转向

## 技巧总结

### 1. 方向数组技巧

- 使用 `dx[]` 和 `dy[]` 简化方向变化
- 模运算实现方向循环

### 2. 边界处理技巧

- 多开一圈数组（10×10处理n≤9），避免复杂边界检查
- 填充时检查 `a[nx][ny] != 0` 判断是否已填充

### 3. 格式化输出技巧

- `setw()` 简单直接
- 手动计算位数，输出前导空格

## 扩展思考

### 1. 逆螺旋矩阵

从外到内填充，方向相反，可修改方向数组实现

### 2. 螺旋遍历

给定矩阵，按螺旋顺序输出，类似逻辑但检查条件不同

### 3. 动态宽度

根据最大数字自动计算所需宽度，增强通用性

## 参考资料

- 洛谷官方题解
- 《算法竞赛入门经典》螺旋矩阵章节
- LeetCode 59. 螺旋矩阵 II
