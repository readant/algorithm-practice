# C++ 算法函数

## 一、排序

```cpp
#include <algorithm>
using namespace std;

vector<int> v = {3, 1, 4, 1, 5};

// 升序排序
sort(v.begin(), v.end());

// 降序排序
sort(v.begin(), v.end(), greater<int>());

// 自定义比较
sort(v.begin(), v.end(), [](int a, int b) {
    return a > b;  // 降序
});
```

## 二、查找

```cpp
vector<int> v = {1, 3, 5, 7, 9};

// 二分查找（需要有序）
auto it = lower_bound(v.begin(), v.end(), 5);  // 第一个>=5
auto it = upper_bound(v.begin(), v.end(), 5);  // 第一个>5

// 线性查找
auto it = find(v.begin(), v.end(), 5);
if (it != v.end()) {
    cout << "找到位置: " << it - v.begin() << endl;
}
```

## 三、数学函数

```cpp
#include <algorithm>

// 最大最小
int mx = max(a, b);
int mn = min(a, b);

// 交换
swap(a, b);

// 绝对值
int abs_val = abs(-5);

// 幂
double p = pow(2, 10);

// 平方根
double s = sqrt(16);

// 最大公约数
int g = gcd(12, 18);  // C++17

// 最小公倍数
int l = lcm(12, 18);  // C++17
```

## 四、去重

```cpp
vector<int> v = {1, 2, 2, 3, 3, 3, 4};

// 先排序
sort(v.begin(), v.end());

// 去重
auto last = unique(v.begin(), v.end());
v.erase(last, v.end());
```

## 五、全排列

```cpp
vector<int> v = {1, 2, 3};

// 生成全排列
do {
    for (int x : v) cout << x << " ";
    cout << endl;
} while (next_permutation(v.begin(), v.end()));
```

## 六、二分查找

```cpp
// 自定义二分
int binary_search(vector<int>& v, int target) {
    int left = 0, right = v.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (v[mid] == target) return mid;
        else if (v[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}
```
