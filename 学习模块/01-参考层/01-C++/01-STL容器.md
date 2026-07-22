# C++ STL 容器

> 详细教程请见 [学习层/01-C++/02-核心语法/07-STL入门.md](../../../02-学习层/01-C++/02-核心语法/07-STL入门.md)

## 一、vector（动态数组）

```cpp
#include <vector>
using namespace std;

// 创建
vector<int> v;
vector<int> v(5, 0);      // 5个0
vector<int> v = {1,2,3};

// 常用操作
v.push_back(x);           // 末尾添加
v.pop_back();             // 末尾删除
v.size();                 // 大小
v.empty();                // 是否为空
v.clear();                // 清空
v.resize(10);             // 调整大小

// 访问
v[i];                     // 下标访问
v.at(i);                  // 带边界检查
v.front();                // 第一个元素
v.back();                 // 最后一个元素

// 遍历
for (int x : v) cout << x << " ";  // C++11
for (auto& x : v) x *= 2;          // 修改
```

## 二、string（字符串）

```cpp
#include <string>
using namespace std;

string s = "hello";

// 常用操作
s.size(); s.length();      // 长度
s.substr(1, 3);            // 子串 [1,4)
s.find("ll");              // 查找位置
s.replace(1, 2, "a");      // 替换
s.insert(2, "xx");         // 插入
s.erase(1, 2);             // 删除

// 类型转换
to_string(123);            // 数字转字符串
stoi("123");               // 字符串转整数
stod("3.14");              // 字符串转浮点数
```

## 三、set（集合）

```cpp
#include <set>
using namespace std;

set<int> s;
s.insert(5);
s.insert(3);
s.insert(5);  // 重复插入无效

// 查找
if (s.find(3) != s.end()) {
    cout << "存在" << endl;
}

// 删除
s.erase(3);

// 遍历（自动排序）
for (int x : s) cout << x << " ";
```

## 四、map（映射）

```cpp
#include <map>
using namespace std;

map<string, int> mp;
mp["apple"] = 5;
mp["banana"] = 3;

// 访问
cout << mp["apple"] << endl;
cout << mp.count("orange") << endl;  // 0或1

// 遍历
for (auto& p : mp) {
    cout << p.first << " " << p.second << endl;
}
```

## 五、queue（队列）

```cpp
#include <queue>
using namespace std;

queue<int> q;
q.push(1);
q.push(2);
q.pop();           // 弹出队首
q.front();         // 队首元素
q.back();          // 队尾元素
q.empty();         // 是否为空
```

## 六、stack（栈）

```cpp
#include <stack>
using namespace std;

stack<int> st;
st.push(1);
st.push(2);
st.pop();          // 弹出栈顶
st.top();          // 栈顶元素
st.empty();        // 是否为空
```

## 七、priority_queue（优先队列）

```cpp
#include <queue>
using namespace std;

// 大顶堆（默认）
priority_queue<int> pq;
pq.push(3);
pq.push(1);
pq.push(5);
cout << pq.top() << endl;  // 5

// 小顶堆
priority_queue<int, vector<int>, greater<int>> min_pq;
```
