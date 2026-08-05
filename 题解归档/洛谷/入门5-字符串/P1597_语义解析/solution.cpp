#include <iostream>   // cin, cout
#include <string>     // string 类型

using namespace std;  // 使用标准命名空间，方便直接使用 cin/cout/string

int main() {
    // 用 ios::sync_with_stdio(false); 关闭 C 和 C++ 流同步，提高 cin/cout 速度
    // 用 cin.tie(nullptr); 解除 cin 和 cout 的绑定，进一步提速
    // 对本题输入量极小，非必须，但养成良好习惯。
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string s;           // 存储输入的 PASCAL 代码串
    cin >> s;           // 直接读取整串，无空格，故无需 getline

    // 用 int 数组存储三个变量的值，下标用字符 'a'、'b'、'c' 直接映射
    // 因为字符在内存中对应 ASCII 码，'a' 的值是 97，所以数组大小设为 128 足够容纳 'c'
    int var[128] = {0}; // 全部初始化为 0，这样未赋值的变量就是 0

    int i = 0;          // 当前扫描位置
    int len = s.length();

    while (i < len) {
        // 1. 取出左变量字符
        char left = s[i];

        // 2. 指针跳过 ":="，即从当前位置往后移动 3 个字符
        //    注意：s[i] 是左变量，s[i+1] 是 ':'，s[i+2] 是 '='，s[i+3] 才是右值
        i = i + 3;

        // 3. 读取右值字符
        char right = s[i];

        // 4. 判断右值是数字还是变量
        if (right >= '0' && right <= '9') {
            // 数字字符转整数：字符 '0' 的 ASCII 码是 48，减去 '0' 得到数值
            var[left] = right - '0';
        } else {
            // 右值是变量，将当前该变量的值赋给 left 变量
            var[left] = var[right];
        }

        // 5. 跳过右值字符和分号，指向下一条语句的开头
        //    右值占 1 位，分号占 1 位，所以 i 向后移动 2
        i = i + 2;
    }

    // 输出结果，用空格隔开，最后换行用 '\n'
    // 注意：不要用 endl，因为它会强制刷新缓冲区，影响性能。
    cout << var['a'] << ' ' << var['b'] << ' ' << var['c'] << '\n';

    return 0;
}