# STL入门

> 快速查阅 STL 容器 API 请见 [参考层/01-C++/01-STL容器.md](../../../01-参考层/01-C++/01-STL容器.md)

## 为什么需要 STL

### 手动管理数组的痛点

```cpp
// 不用 STL，你需要手动管理内存
int arr[100];      // 固定大小，不知道够不够用
int size = 0;      // 需要自己追踪元素个数

void pushBack(int val) {
    if (size >= 100) return;  // 满了就加不进去
    arr[size++] = val;
}

void insert(int index, int val) {
    // 要自己写移动元素的逻辑
    for (int i = size; i > index; i--) {
        arr[i] = arr[i - 1];
    }
    arr[index] = val;
    size++;
}
```

**问题**：固定大小、手动扩容、代码冗余、容易出错。

### STL 解决了什么

| 问题 | STL 解决方案 |
|------|-------------|
| 固定大小 | `vector` 动态扩容 |
| 手动移动元素 | `insert()` 自动处理 |
| 查找效率低 | `unordered_map` O(1) 查找 |
| 排序重复实现 | `sort()` 一行搞定 |
| 栈/队列手写 | `stack`/`queue` 直接用 |

---

## STL 是什么

STL（Standard Template Library）是 C++ 标准库，提供四大组件：

| 组件 | 作用 | 常用类型 |
|------|------|----------|
| **容器** | 存储数据 | vector, map, set, stack, queue |
| **迭代器** | 遍历容器 | iterator, const_iterator |
| **算法** | 操作数据 | sort, find, binary_search |
| **函数对象** | 自定义行为 | greater, less, lambda |

---

## 何时用什么容器

```
需要动态数组？        → vector
需要去重？            → set
需要键值映射？        → map / unordered_map
需要先进先出？        → queue
需要后进先出？        → stack
需要快速查找+排序？   → map（有序）/ unordered_map（更快）
需要维护最大/最小值？ → priority_queue
```

---

## 容器

### 序列容器

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <deque>
using namespace std;

int main() {
    // vector：动态数组，支持随机访问
    vector<int> v = {1, 2, 3, 4, 5};
    v.push_back(6);
    v.pop_back();
    cout << "Vector: ";
    for (int x : v) cout << x << " ";
    cout << endl;
    
    // list：双向链表，支持快速插入删除
    list<int> l = {1, 2, 3};
    l.push_front(0);
    l.push_back(4);
    cout << "List: ";
    for (int x : l) cout << x << " ";
    cout << endl;
    
    // deque：双端队列
    deque<int> d = {1, 2, 3};
    d.push_front(0);
    d.push_back(4);
    cout << "Deque: ";
    for (int x : d) cout << x << " ";
    cout << endl;
    
    return 0;
}
```

### 关联容器

```cpp
#include <iostream>
#include <set>
#include <map>
using namespace std;

int main() {
    // set：有序集合，自动去重
    set<int> s = {3, 1, 4, 1, 5};
    s.insert(2);
    s.erase(4);
    cout << "Set: ";
    for (int x : s) cout << x << " ";
    cout << endl;
    
    // multiset：允许重复
    multiset<int> ms = {1, 1, 2, 3};
    cout << "Multiset count(1): " << ms.count(1) << endl;
    
    // map：有序键值对
    map<string, int> m;
    m["Alice"] = 90;
    m["Bob"] = 85;
    m["Charlie"] = 95;
    cout << "Map:" << endl;
    for (auto& [key, value] : m) {
        cout << "  " << key << ": " << value << endl;
    }
    
    // unordered_map：哈希表，O(1)查找
    unordered_map<string, int> um;
    um["hello"] = 1;
    um["world"] = 2;
    
    return 0;
}
```

### 适配器容器

```cpp
#include <iostream>
#include <stack>
#include <queue>
using namespace std;

int main() {
    // stack：栈（LIFO）
    stack<int> st;
    st.push(1);
    st.push(2);
    st.push(3);
    cout << "Stack top: " << st.top() << endl;
    st.pop();
    cout << "Stack top after pop: " << st.top() << endl;
    
    // queue：队列（FIFO）
    queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    cout << "Queue front: " << q.front() << endl;
    q.pop();
    cout << "Queue front after pop: " << q.front() << endl;
    
    // priority_queue：优先队列（堆）
    priority_queue<int> pq;  // 最大堆
    pq.push(3);
    pq.push(1);
    pq.push(4);
    cout << "Priority queue top: " << pq.top() << endl;
    
    // 最小堆
    priority_queue<int, vector<int>, greater<int>> minPq;
    minPq.push(3);
    minPq.push(1);
    minPq.push(4);
    cout << "Min heap top: " << minPq.top() << endl;
    
    return 0;
}
```

## 迭代器

```cpp
#include <iostream>
#include <vector>
#include <map>
using namespace std;

int main() {
    vector<int> v = {10, 20, 30, 40, 50};
    
    // 正向迭代器
    cout << "Forward: ";
    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    // 反向迭代器
    cout << "Backward: ";
    for (vector<int>::reverse_iterator it = v.rbegin(); it != v.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    // const迭代器
    const vector<int>& cv = v;
    cout << "Const: ";
    for (auto it = cv.cbegin(); it != cv.cend(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    return 0;
}
```

## 常用算法

### 排序与查找

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    vector<int> v = {5, 3, 1, 4, 2};
    
    // 排序
    sort(v.begin(), v.end());
    cout << "Sorted: ";
    for (int x : v) cout << x << " ";
    cout << endl;
    
    // 降序排序
    sort(v.begin(), v.end(), greater<int>());
    cout << "Descending: ";
    for (int x : v) cout << x << " ";
    cout << endl;
    
    // 二分查找（需有序）
    bool found = binary_search(v.begin(), v.end(), 3);
    cout << "Found 3: " << found << endl;
    
    // 查找第一个>=3的位置
    auto it = lower_bound(v.begin(), v.end(), 3);
    cout << "Lower bound of 3: " << *it << endl;
    
    // 查找第一个>3的位置
    auto it2 = upper_bound(v.begin(), v.end(), 3);
    cout << "Upper bound of 3: " << *it2 << endl;
    
    return 0;
}
```

### 数值算法

```cpp
#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3, 4, 5};
    
    // 求和
    int sum = accumulate(v.begin(), v.end(), 0);
    cout << "Sum: " << sum << endl;
    
    // 前缀和
    vector<int> prefix(v.size());
    partial_sum(v.begin(), v.end(), prefix.begin());
    cout << "Prefix sum: ";
    for (int x : prefix) cout << x << " ";
    cout << endl;
    
    // 内积
    vector<int> v2 = {2, 3, 4, 5, 6};
    int dot = inner_product(v.begin(), v.end(), v2.begin(), 0);
    cout << "Dot product: " << dot << endl;
    
    return 0;
}
```

### 修改算法

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3, 4, 5};
    
    // 反转
    reverse(v.begin(), v.end());
    cout << "Reversed: ";
    for (int x : v) cout << x << " ";
    cout << endl;
    
    // 旋转
    rotate(v.begin(), v.begin() + 2, v.end());
    cout << "Rotated: ";
    for (int x : v) cout << x << " ";
    cout << endl;
    
    // 全排列
    vector<int> perm = {1, 2, 3};
    do {
        for (int x : perm) cout << x << " ";
        cout << endl;
    } while (next_permutation(perm.begin(), perm.end()));
    
    return 0;
}
```

## Lambda表达式

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};
    
    // 基本lambda
    sort(v.begin(), v.end(), [](int a, int b) {
        return a > b;
    });
    
    cout << "Sorted desc: ";
    for (int x : v) cout << x << " ";
    cout << endl;
    
    // 捕获变量
    int threshold = 4;
    auto count = [threshold](vector<int>& vec) {
        int cnt = 0;
        for (int x : vec) {
            if (x > threshold) cnt++;
        }
        return cnt;
    };
    
    cout << "Count > " << threshold << ": " << count(v) << endl;
    
    // 通用比较器
    auto cmp = [](const string& a, const string& b) {
        return a.length() < b.length();
    };
    
    vector<string> words = {"apple", "hi", "banana", "ok"};
    sort(words.begin(), words.end(), cmp);
    for (const auto& w : words) cout << w << " ";
    cout << endl;
    
    return 0;
}
```

## 练习题

### 题目1：前k个高频元素
**题目描述**：给定一个非空整数数组，返回出现频率前k高的元素。

**参考代码**：
```cpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> freq;
    for (int x : nums) freq[x]++;
    
    // 最小堆
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    
    for (auto& [num, count] : freq) {
        pq.push({count, num});
        if (pq.size() > k) pq.pop();
    }
    
    vector<int> result;
    while (!pq.empty()) {
        result.push_back(pq.top().second);
        pq.pop();
    }
    
    return result;
}

int main() {
    vector<int> nums = {1, 1, 1, 2, 2, 3};
    int k = 2;
    
    vector<int> result = topKFrequent(nums, k);
    for (int x : result) cout << x << " ";
    cout << endl;
    
    return 0;
}
```

### 题目2：两数之和（STL版本）
**题目描述**：使用unordered_map实现两数之和。

**参考代码**：
```cpp
#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> map;
    
    for (int i = 0; i < nums.size(); i++) {
        int complement = target - nums[i];
        if (map.count(complement)) {
            return {map[complement], i};
        }
        map[nums[i]] = i;
    }
    
    return {};
}

int main() {
    vector<int> nums = {2, 7, 11, 15};
    int target = 9;
    
    auto result = twoSum(nums, target);
    cout << result[0] << ", " << result[1] << endl;
    
    return 0;
}
```

## 小结

1. `vector`是最常用的序列容器
2. `map`/`unordered_map`用于键值映射
3. `priority_queue`用于维护最大/最小值
4. 算法头文件提供sort、find、binary_search等常用算法
5. Lambda表达式可简化回调函数定义
