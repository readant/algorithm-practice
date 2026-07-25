# 经典DP模型

## 1. 背包问题

### 0-1背包（每个物品只能选一次）

```cpp
int knapsack01(vector<int>& weights, vector<int>& values, int capacity) {
    int n = weights.size();
    vector<vector<int>> dp(n + 1, vector<int>(capacity + 1, 0));
    
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= capacity; j++) {
            dp[i][j] = dp[i - 1][j];  // 不选第i个物品
            
            if (j >= weights[i - 1]) {
                dp[i][j] = max(dp[i][j], 
                    dp[i - 1][j - weights[i - 1]] + values[i - 1]);
            }
        }
    }
    
    return dp[n][capacity];
}
```

### 完全背包（每个物品可选无限次）

```cpp
int knapsackComplete(vector<int>& weights, vector<int>& values, int capacity) {
    int n = weights.size();
    vector<int> dp(capacity + 1, 0);
    
    for (int i = 0; i < n; i++) {
        for (int j = weights[i]; j <= capacity; j++) {
            dp[j] = max(dp[j], dp[j - weights[i]] + values[i]);
        }
    }
    
    return dp[capacity];
}
```

---

## 2. 线性DP

### 最长递增子序列（LIS）

```cpp
int lengthOfLIS(vector<int>& nums) {
    int n = nums.size();
    vector<int> dp(n, 1);
    
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i]) {
                dp[i] = max(dp[i], dp[j] + 1);
            }
        }
    }
    
    return *max_element(dp.begin(), dp.end());
}
```

### 最长公共子序列（LCS）

```cpp
int longestCommonSubsequence(string text1, string text2) {
    int n = text1.size(), m = text2.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));
    
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (text1[i - 1] == text2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    
    return dp[n][m];
}
```

---

## 3. 区间DP

### 矩阵链乘法

```cpp
int matrixChain(vector<int>& dims) {
    int n = dims.size() - 1;
    vector<vector<int>> dp(n + 1, vector<int>(n + 1, 0));
    
    for (int len = 2; len <= n; len++) {
        for (int i = 1; i <= n - len + 1; i++) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;
            
            for (int k = i; k < j; k++) {
                dp[i][j] = min(dp[i][j],
                    dp[i][k] + dp[k + 1][j] + dims[i - 1] * dims[k] * dims[j]);
            }
        }
    }
    
    return dp[1][n];
}
```

---

## 背包问题变体

| 类型 | 特点 | 状态转移 |
|------|------|----------|
| 0-1背包 | 每个物品选一次 | `dp[j] = max(dp[j], dp[j-w]+v)` |
| 完全背包 | 每个物品无限次 | 正序遍历j |
| 多重背包 | 每个物品有数量限制 | 二进制拆分 |
| 分组背包 | 物品分组，每组选一个 | 三重循环 |
