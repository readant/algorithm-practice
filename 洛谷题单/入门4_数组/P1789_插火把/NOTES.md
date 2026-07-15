# P1789【Mc生存】插火把 - 通用算法总结

## 一、题目分析

### 1.1 问题描述
n×n方阵中放置m个火把和k个萤石，各有固定照明范围，求**未被照亮且无物品的格子数**。

### 1.2 数据范围
- n ≤ 100，m+k ≤ 25
- 适合O(n²)模拟算法

---

## 二、通用解题方法：二维网格模拟

### 2.1 适用题型
| 题型特征 | 示例 |
|---------|------|
| 网格上的区域覆盖 | 本题、激光炸弹 |
| 网格上的扩散/BFS | 生命之树、细胞分裂 |
| 网格上的标记问题 | 海战、棋盘覆盖 |
| 二维前缀和 | 激光炸弹、差分 |

### 2.2 通用框架

```cpp
// 1. 定义网格
int grid[N][N] = {0};

// 2. 处理每个操作/光源
for (每个操作) {
    读取位置(x, y)
    // 标记影响范围（相对坐标法）
    for (每个相对偏移(dx, dy)) {
        int nx = x + dx, ny = y + dy;
        if (边界检查通过) {
            grid[nx][ny] += 1;  // 或标记为true
        }
    }
}

// 3. 统计结果
for (遍历整个网格) {
    if (满足条件) count++;
}
```

### 2.3 相对坐标法（核心技巧）

**为什么用相对坐标？**
- 将复杂的形状抽象为偏移量数组
- 代码简洁，易于扩展
- 避免重复计算

**本题示例：**
```cpp
// 火把：十字形+四角，共13格
int dx[] = {-2,-1,-1,-1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
int dy[] = { 0,-1, 0, 1,-2,-1, 0, 1, 2,-1, 0, 1, 0};

// 萤石：5×5正方形，共25格
for (int i = -2; i <= 2; i++)
    for (int j = -2; j <= 2; j++) { ... }
```

### 2.4 边界检查模板

```cpp
// 方法1：坐标从1开始，数组开n+2
bool isValid(int x, int y, int n) {
    return x >= 1 && x <= n && y >= 1 && y <= n;
}

// 方法2：数组开大一圈，天然越界安全
bool light[N+2][N+2];  // 但要从(1,1)开始使用
```

---

## 三、本题算法详解

### 3.1 照明范围

| 光源 | 形状 | 格子数 | 相对坐标 |
|------|------|--------|----------|
| 火把 | 十字+四角 | 13 | 见代码 |
| 萤石 | 5×5正方形 | 25 | (-2,-2)到(2,2) |

### 3.2 核心逻辑

```
1. 初始化 light[][] = false, item[][] = false
2. 处理火把：标记item，标记照亮范围
3. 处理萤石：标记item，标记照亮范围
4. 统计：!light[i][j] && !item[i][j] 的数量
```

### 3.3 易错点

1. **物品位置不算怪物**：即使被照亮，物品位置也要标记
2. **照明可重叠**：重叠不影响结果，只需标记"是否被照亮"
3. **坐标从1开始**：不是从0开始
4. **边界检查**：照明位置可能超出1~n范围

---

## 四、多语言实现对比

### 4.1 代码量对比

| 语言 | 代码行数 | 文件 |
|------|---------|------|
| Python | 53行 | P1789_插火把.py |
| C++ | 71行 | P1789_插火把.cpp |
| C | 75行 | P1789_插火把.c |
| Java | 75行 | P1789_插火把.java |

### 4.2 输入方式对比

| 语言 | 代码示例 | 特点 |
|------|---------|------|
| **C++** | `cin >> n >> m >> k;` | 流式输入，自动类型推导 |
| **C** | `scanf("%d %d %d", &n, &m, &k);` | 格式化输入，需要取地址 |
| **Python** | `n, m, k = map(int, input().split())` | 一行搞定，最简洁 |
| **Java** | `scanner.nextInt()` | 面向对象，需要创建Scanner对象 |

### 4.3 数组声明对比

| 语言 | 代码示例 | 特点 |
|------|---------|------|
| **C++** | `vector<vector<bool>> light(n+1, vector<bool>(n+1, false))` | 动态分配，自动管理内存 |
| **C** | `bool light[MAX_N][MAX_N]` + `#define MAX_N 102` | 静态分配，需要预定义大小 |
| **Python** | `light = [[False] * (n+1) for _ in range(n+1)]` | 列表推导式，最灵活 |
| **Java** | `boolean[][] light = new boolean[n+1][n+1]` | 动态分配，但语法较冗长 |

### 4.4 相对坐标存储对比

| 语言 | 代码示例 | 特点 |
|------|---------|------|
| **C++** | 两个独立数组 `torchDx[]` 和 `torchDy[]` | 传统C风格，高效 |
| **C** | 同C++，全局数组 | 全局变量，作用域大 |
| **Python** | 元组列表 `torch_offsets = [(-2,0), (-1,-1), ...]` | Pythonic，解包方便 |
| **Java** | 两个独立数组 `torchDx[]` 和 `torchDy[]` | 同C++风格 |

### 4.5 循环遍历对比

| 语言 | 代码示例 | 特点 |
|------|---------|------|
| **C++** | `for (int j = 0; j < torchCount; j++)` | 传统for循环 |
| **C** | 同C++ | 最底层控制 |
| **Python** | `for dx, dy in torch_offsets:` | 迭代器解包，最优雅 |
| **Java** | `for (int j = 0; j < torchDx.length; j++)` | 同C++，但用.length |

### 4.6 边界检查对比

| 语言 | 代码示例 | 特点 |
|------|---------|------|
| **C++** | `if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)` | 逻辑与 |
| **C** | 同C++ | 最高效 |
| **Python** | `if 1 <= nx <= n and 1 <= ny <= n:` | 链式比较，最Pythonic |
| **Java** | 同C++ | 逻辑与 |

### 4.7 布尔类型对比

| 语言 | 布尔值 | 取反 |
|------|-------|------|
| C++ | `true` / `false` | `!light[i][j]` |
| C | `true` / `false` (需要stdbool.h) | `!light[i][j]` |
| Python | `True` / `False` | `not light[i][j]` |
| Java | `true` / `false` | `!light[i][j]` |

### 4.8 数组越界处理对比

| 语言 | 策略 | 说明 |
|------|------|------|
| C++ | `vector` 自动检查 | 可用 `.at()` 抛异常 |
| C | 无保护 | 越界是未定义行为 |
| Python | 抛出 `IndexError` | 严格检查 |
| Java | 抛出 `ArrayIndexOutOfBoundsException` | 严格检查 |

---

## 五、性能对比

### 5.1 执行效率（估算）

```
测试环境：n=100, m=25, k=5

C:      ~0.1ms  (最快)
C++:    ~0.2ms  (接近C)
Java:   ~5ms    (JVM启动开销)
Python: ~10ms   (解释执行)
```

### 5.2 内存使用

| 语言 | 内存分配方式 | 开销 |
|------|-------------|------|
| C | 静态/栈 | 最小 |
| C++ | vector (堆) | 较小 |
| Java | 对象数组 (堆) | 中等 |
| Python | 列表对象 (堆) | 最大 |

---

## 六、代码风格对比

### 6.1 C++ - 平衡型
```cpp
// 优点：效率高，语法现代
// 缺点：需要手动管理部分细节
vector<vector<bool>> light(n + 1, vector<bool>(n + 1, false));
for (int j = 0; j < torchCount; j++) {
    int nx = x + torchDx[j];
    int ny = y + torchDy[j];
    if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
        light[nx][ny] = true;
}
```

### 6.2 C - 底层型
```c
// 优点：最高效，完全控制
// 缺点：代码冗长，容易出错
#define MAX_N 102
bool light[MAX_N][MAX_N];
// 需要手动初始化
for (int i = 1; i <= n; i++)
    for (int j = 1; j <= n; j++)
        light[i][j] = false;
```

### 6.3 Python - 简洁型
```python
# 优点：代码最短，可读性最好
# 缺点：性能最差
light = [[False] * (n + 1) for _ in range(n + 1)]
for dx, dy in torch_offsets:
    nx, ny = x + dx, y + dy
    if 1 <= nx <= n and 1 <= ny <= n:
        light[nx][ny] = True
```

### 6.4 Java - 规范型
```java
// 优点：类型安全，面向对象
// 缺点：代码最冗长
boolean[][] light = new boolean[n + 1][n + 1];
for (int j = 0; j < torchDx.length; j++) {
    int nx = x + torchDx[j];
    int ny = y + torchDy[j];
    if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
        light[nx][ny] = true;
}
```

---

## 七、四种语言完整代码

### 7.1 C++版本
```cpp
#include <iostream>
#include <vector>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m, k;
    cin >> n >> m >> k;
    
    vector<vector<bool>> light(n + 1, vector<bool>(n + 1, false));
    vector<vector<bool>> item(n + 1, vector<bool>(n + 1, false));
    
    int torchDx[] = {-2,-1,-1,-1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
    int torchDy[] = { 0,-1, 0, 1,-2,-1, 0, 1, 2,-1, 0, 1, 0};
    
    vector<int> glowDx, glowDy;
    for (int i = -2; i <= 2; i++)
        for (int j = -2; j <= 2; j++) {
            glowDx.push_back(i);
            glowDy.push_back(j);
        }
    
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
        for (int j = 0; j < glowDx.size(); j++) {
            int nx = x + glowDx[j], ny = y + glowDy[j];
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
```

### 7.2 C版本
```c
#include <stdio.h>
#include <stdbool.h>

#define MAX_N 102

bool light[MAX_N][MAX_N];
bool item[MAX_N][MAX_N];

int torch_dx[] = {-2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
int torch_dy[] = {0, -1, 0, 1, -2, -1, 0, 1, 2, -1, 0, 1, 0};

int glow_dx[] = {-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2};
int glow_dy[] = {-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2};

int main() {
    int n, m, k;
    scanf("%d %d %d", &n, &m, &k);
    
    for (int i = 0; i < m; i++) {
        int x, y; scanf("%d %d", &x, &y);
        item[x][y] = true;
        for (int j = 0; j < 13; j++) {
            int nx = x + torch_dx[j], ny = y + torch_dy[j];
            if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
                light[nx][ny] = true;
        }
    }
    
    for (int i = 0; i < k; i++) {
        int x, y; scanf("%d %d", &x, &y);
        item[x][y] = true;
        for (int j = 0; j < 25; j++) {
            int nx = x + glow_dx[j], ny = y + glow_dy[j];
            if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
                light[nx][ny] = true;
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
```

### 7.3 Python版本
```python
def main():
    n, m, k = map(int, input().split())
    
    light = [[False] * (n + 1) for _ in range(n + 1)]
    item = [[False] * (n + 1) for _ in range(n + 1)]
    
    torch_offsets = [
        (-2, 0), (-1, -1), (-1, 0), (-1, 1),
        (0, -2), (0, -1), (0, 0), (0, 1), (0, 2),
        (1, -1), (1, 0), (1, 1), (2, 0)
    ]
    
    glow_offsets = [(i, j) for i in range(-2, 3) for j in range(-2, 3)]
    
    for _ in range(m):
        x, y = map(int, input().split())
        item[x][y] = True
        for dx, dy in torch_offsets:
            nx, ny = x + dx, y + dy
            if 1 <= nx <= n and 1 <= ny <= n:
                light[nx][ny] = True
    
    for _ in range(k):
        x, y = map(int, input().split())
        item[x][y] = True
        for dx, dy in glow_offsets:
            nx, ny = x + dx, y + dy
            if 1 <= nx <= n and 1 <= ny <= n:
                light[nx][ny] = True
    
    monster_count = 0
    for i in range(1, n + 1):
        for j in range(1, n + 1):
            if not light[i][j] and not item[i][j]:
                monster_count += 1
    
    print(monster_count)

if __name__ == "__main__":
    main()
```

### 7.4 Java版本
```java
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        int n = scanner.nextInt();
        int m = scanner.nextInt();
        int k = scanner.nextInt();
        
        boolean[][] light = new boolean[n + 1][n + 1];
        boolean[][] item = new boolean[n + 1][n + 1];
        
        int[] torchDx = {-2,-1,-1,-1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
        int[] torchDy = { 0,-1, 0, 1,-2,-1, 0, 1, 2,-1, 0, 1, 0};
        
        int[] glowDx = new int[25];
        int[] glowDy = new int[25];
        int index = 0;
        for (int i = -2; i <= 2; i++)
            for (int j = -2; j <= 2; j++) {
                glowDx[index] = i;
                glowDy[index] = j;
                index++;
            }
        
        for (int i = 0; i < m; i++) {
            int x = scanner.nextInt(), y = scanner.nextInt();
            item[x][y] = true;
            for (int j = 0; j < torchDx.length; j++) {
                int nx = x + torchDx[j], ny = y + torchDy[j];
                if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
                    light[nx][ny] = true;
            }
        }
        
        for (int i = 0; i < k; i++) {
            int x = scanner.nextInt(), y = scanner.nextInt();
            item[x][y] = true;
            for (int j = 0; j < glowDx.length; j++) {
                int nx = x + glowDx[j], ny = y + glowDy[j];
                if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
                    light[nx][ny] = true;
            }
        }
        
        int ans = 0;
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= n; j++)
                if (!light[i][j] && !item[i][j])
                    ans++;
        
        System.out.println(ans);
        scanner.close();
    }
}
```

---

## 八、扩展：类似题目通用解法

### 8.1 激光炸弹（洛谷P2280）
- **题意**：多个炸弹炸矩形区域，求最大价值
- **解法**：二维前缀和
```cpp
// 构建前缀和
for (int i = 1; i <= n; i++)
    for (int j = 1; j <= m; j++)
        sum[i][j] = sum[i-1][j] + sum[i][j-1] - sum[i-1][j-1] + a[i][j];

// 查询矩形区域和
int query(int x1, int y1, int x2, int y2) {
    return sum[x2][y2] - sum[x1-1][y2] - sum[x2][y1-1] + sum[x1-1][y1-1];
}
```

### 8.2 海战（洛谷P1312）
- **题意**：在网格上放置战舰
- **解法**：标记+模拟，与本题类似

### 8.3 生命之树（洛谷P1352）
- **题意**：树形DP
- **解法**：DFS+状态转移

---

## 九、通用模板：网格模拟题

```cpp
#include <bits/stdc++.h>
using namespace std;

const int N = 105;
int n, m;
bool vis[N][N];

struct Point {
    int dx, dy;
};

vector<Point> shape1 = {{0,0}, {0,1}, {1,0}, {1,1}};  // 2x2正方形
vector<Point> shape2 = {{0,0}, {0,1}, {0,2}, {1,1}, {2,1}};  // T字形

void mark(int x, int y, vector<Point>& shape) {
    for (auto& p : shape) {
        int nx = x + p.dx, ny = y + p.dy;
        if (nx >= 1 && nx <= n && ny >= 1 && ny <= m) {
            vis[nx][ny] = true;
        }
    }
}

int main() {
    // 读取输入
    // 处理每个操作
    // 统计结果
    return 0;
}
```

---

## 十、复杂度分析

| 操作 | 时间复杂度 | 说明 |
|------|-----------|------|
| 标记一个光源 | O(S) | S=光源覆盖格子数 |
| 处理所有光源 | O(K×S) | K=光源数量 |
| 统计结果 | O(N²) | N=网格大小 |
| **总复杂度** | **O(N² + K×S)** | 空间O(N²) |

---

## 十一、选择建议

| 场景 | 推荐语言 | 原因 |
|------|---------|------|
| 竞赛/ACM | C++ | 效率高，STL方便 |
| 系统编程 | C | 最底层控制 |
| 快速原型 | Python | 代码最短 |
| 企业开发 | Java | 规范性强 |
| 学习入门 | Python | 最易理解 |

---

## 十二、总结

### 解题三步法
1. **识别题型**：网格覆盖/标记问题 → 相对坐标法
2. **定义形状**：将覆盖范围转化为相对坐标数组
3. **模拟标记**：逐个处理，边界检查，最后统计

### 多语言核心差异
| 方面 | C/C++ | Python | Java |
|------|-------|--------|------|
| **性能** | 最快 | 最慢 | 中等 |
| **代码量** | 中等 | 最少 | 最多 |
| **内存管理** | 手动/半自动 | 自动 | 自动 |
| **类型安全** | 弱/中 | 弱 | 强 |

### 关键结论
- **算法完全相同**，差异仅在于语法细节
- **相对坐标法是语言无关的**，掌握后可直接移植
- 选择语言应根据具体场景和需求

**记住**：二维网格问题，相对坐标是万能钥匙！

---

## 十三、刷题技巧

### 13.1 做题前准备

| 步骤 | 做什么 | 为什么 |
|------|--------|--------|
| 1 | 读题2-3遍 | 理解题意，避免遗漏条件 |
| 2 | 画图/举例 | 把抽象问题具体化 |
| 3 | 写出输入输出样例的推导过程 | 验证理解是否正确 |
| 4 | 分析数据范围 | 决定算法复杂度上限 |

### 13.2 遇到不会的题怎么办

```
1. 先看数据范围
   → n ≤ 20: 暴力/搜索
   → n ≤ 1000: O(n²) 动态规划
   → n ≤ 100000: O(n log n) 排序/二分
   → n ≤ 1000000: O(n) 线性算法

2. 再看题目类型关键词
   → "网格/方阵" → 二维数组 + 相对坐标
   → "最短/最少" → BFS/Dijkstra
   → "方案数" → DP
   → "判断是否" → 模拟/搜索

3. 最后看样例
   → 手动模拟样例推导过程
   → 找出规律
```

### 13.3 调试技巧

| 问题类型 | 调试方法 |
|---------|---------|
| 输出不对 | 打印中间变量，对比样例推导 |
| 编译错误 | 从第一个错误开始修，后面的可能是连锁反应 |
| 运行超时 | 检查循环边界，避免死循环 |
| 内存超限 | 检查数组大小，避免开太大 |
| 答案错误 | 检查边界条件，特殊情况 |

### 13.4 常见错误清单

```
□ 数组越界（下标从0还是1开始？）
□ 循环边界（< 还是 <=？）
□ 变量未初始化
□ 整数溢出（int还是long long？）
□ 输入格式错误（多读/少读）
□ 忘记处理特殊情况（k=0时？）
□ 边界检查遗漏
```

---

## 十四、提醒总结

### 14.1 本题核心提醒

| 提醒 | 说明 |
|------|------|
| **坐标从1开始** | 不是从0开始，数组要开n+1 |
| **边界检查必须加** | 照明位置可能超出1~n范围 |
| **物品位置要标记** | item数组不能忘 |
| **照明可重叠** | 重叠不影响，只需标记是否照亮 |
| **火把和萤石范围不同** | 分别处理，不要搞混 |

### 14.2 网格题通用提醒

| 提醒 | 说明 |
|------|------|
| **先画图再写代码** | 抽象问题具体化 |
| **相对坐标是核心** | 把形状转化为偏移量数组 |
| **边界检查不能忘** | 否则可能越界或漏算 |
| **用bool数组标记** | visited/light/flag等 |
| **初始化要完整** | 不要漏掉某个数组 |

### 14.3 编程习惯提醒

| 习惯 | 重要性 |
|------|--------|
| **先想清楚再写** | 避免返工 |
| **边写边测** | 小步验证，及时发现问题 |
| **看数据范围定算法** | 避免超时 |
| **处理完一种情况再处理下一种** | 逻辑清晰 |
| **提交前检查样例** | 最基本的验证 |

### 14.4 学习路径提醒

```
入门阶段（现在）:
  → 数组、循环、条件判断
  → 模拟题、简单数学题

进阶阶段:
  → 排序、二分查找
  → 字符串处理
  → 简单数据结构（栈、队列、链表）

高级阶段:
  → 搜索（DFS/BFS）
  → 动态规划
  → 图论基础

竞赛阶段:
  → 高级数据结构
  → 复杂算法
  → 优化技巧
```

### 14.5 刷题建议

| 建议 | 说明 |
|------|------|
| **按题单刷** | 系统学习，循序渐进 |
| **每题写总结** | 记录思路和易错点 |
| **同类题一起刷** | 巩固知识点 |
| **不要只看题解** | 先自己思考30分钟 |
| **定期回顾错题** | 避免重复犯错 |

---

## 十五、本题难度评估

| 评估项 | 评分（5分制） |
|--------|--------------|
| 思维难度 | ⭐⭐ (2/5) |
| 代码难度 | ⭐⭐ (2/5) |
| 知识点难度 | ⭐ (1/5) |
| 综合难度 | ⭐⭐ (入门级) |

**结论**：这道题是**入门级**数组题，主要考察：
1. 二维数组的使用
2. 模拟法
3. 边界处理

做不出来很正常，可能是：
- 相对坐标法不熟悉
- 题目信息量大，没理清思路
- 边界检查容易忘

**建议**：多做几道类似的网格题，熟悉套路后就能轻松应对！