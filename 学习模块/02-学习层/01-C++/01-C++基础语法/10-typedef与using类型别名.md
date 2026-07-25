## typedef
在C++中有一个关键字和类型有关的，是用来给类型重命名的。当有一个类型比较复杂的时候，可以简化类型。`typedef`  在竞赛中经常使用，可以提升编码速度，typedef使用的基本语法形式：

```cpp
    typedef 旧类型名 新类型名
```

比如：
```cpp
typedef unsigned int uint;
typedef long long ll;
typedef unsigned long long ull;
```
上面代码的意思是将 `unsigned int` 类型重命名为`uint`,使用`uint`创建变量和使用``unsigned int`是一样的，其他几个也是一样的道理。



## 1. 基础速写
```cpp
typedef unsigned int uint;      // 以后 uint 就是 unsigned int
typedef long long ll;           // 竞赛标配，少打 7 个字符
typedef unsigned long long ull; // 最大的整数类型

// 也可以给结构体起外号（省去写 struct 前缀）
typedef struct { 
    int x; 
    int y; 
} Point; 

// 使用：
Point p1;  // 完全等价于 struct {int x; int y;} p1; 一下子方便多了
```

## 2. 数组类型(更便捷的参数传递)
当需要一个固定长度的数组类型时，把它 `typedef` ,后面用起来会非常方便。

```cpp
typedef int FiveInts[5];

// 使用：
FiveInts arr1; // 等价于 int arr1[5];
FiveInts arr2; // 等价于 inte arr2[5];

// 特别适用于二维数组传参
typedef int Matrix[4][4];
Matrix mat;    // 等价于 int mat[4][4];

```

## 3. 给“函数指针”起外号

普通函数
这个函数叫 `add`,类型是"接受两个`int`,返回一个 `int`" 
```cpp
int add(int a,int b) {return a + b;}
```
声明一个“指向这种函数的指针变量”,如果我想声明一个指针变量 `p` ，让它能指向 `add`，C++的语法就是：
```cpp
int (*p)(int,int);
```
为什么括号这么变态？
因为如果不加括号写成 `int *p(int,int);`，编译器会认为："这是一个函数，名字叫做`p`，返回值是`int*`（指针）"
所以

---

## using 类型别名（C++11）

`using` 是 C++11 引入的类型别名语法，比 `typedef` 更直观，尤其在复杂类型声明上可读性更强。

### 基本语法

```cpp
using 新类型名 = 旧类型名;
```

### 对比 typedef

```cpp
// typedef 写法
typedef long long ll;
typedef int (*FuncPtr)(int, int);

// using 写法（更直观）
using ll = long long;
using FuncPtr = int (*)(int, int);
```

### 模板别名（using 独有优势）

`using` 可以给模板定义别名，这是 `typedef` 做不到的：

```cpp
// 定义通用类型别名
template <typename T>
using Vec = std::vector<T>;

// 使用
Vec<int> v;           // 等价于 std::vector<int>
Vec<std::string> vs;  // 等价于 std::vector<std::string>

// 给 map 起别名
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

HashMap<int, string> m;  // 等价于 std::unordered_map<int, string>
```

### 竞赛中常用别名

```cpp
using ll = long long;
using pii = std::pair<int, int>;
using pll = std::pair<long long, long long>;
using vi = std::vector<int>;
using vll = std::vector<long long>;

// 配合模板别名更方便
template <typename T>
using v = std::vector<T>;

v<int> a(10);      // vector<int>(10)
v<pii> edges;      // vector<pair<int, int>>
```

### 小结

| 特性 | typedef | using |
|------|---------|-------|
| 基本别名 | `typedef int ll;` | `using ll = int;` |
| 函数指针 | 较复杂 | 更直观 |
| 模板别名 | 不支持 | 支持 |
| 推荐程度 | 兼容老代码 | C++11+ 首选 |

