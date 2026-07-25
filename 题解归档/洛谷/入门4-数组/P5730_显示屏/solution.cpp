#include <iostream>
#include <string>  // 引入字符串库，使用string类型
using namespace std;
// 定义常量二维数组，存储 0~9每个数字的5行点阵图案
// const 表示数组的每个string对象都是常量。即初始化之后不能被修改
// num[数字][行号] 是一个 string ,每个string 长度为3
const string num[10][5] = {
  // 数字 0
    {"XXX", "X.X", "X.X", "X.X", "XXX"},
    // 数字 1
    {"..X", "..X", "..X", "..X", "..X"},
    // 数字 2
    {"XXX", "..X", "XXX", "X..", "XXX"},
    // 数字 3
    {"XXX", "..X", "XXX", "..X", "XXX"},
    // 数字 4
    {"X.X", "X.X", "XXX", "..X", "..X"},
    // 数字 5
    {"XXX", "X..", "XXX", "..X", "XXX"},
    // 数字 6
    {"XXX", "X..", "XXX", "X.X", "XXX"},
    // 数字 7
    {"XXX", "..X", "..X", "..X", "..X"},
    // 数字 8
    {"XXX", "X.X", "XXX", "X.X", "XXX"},
    // 数字 9
    {"XXX", "X.X", "XXX", "..X", "XXX"}
};

int main() {
    // 提升输入输出速度，常用于竞赛编程
    // 取消 cin 与 stdio 的同步，使 cin 更快
    ios::sync_with_stdio(0);
    //  将 cin 与 cout 的流绑定解开，防止 cin 之前刷新 cout 缓冲区
    cin.tie(0);

    int n;    // n 表示数字的位数
    cin >> n; // 读取n，虽然之后没有直接使用n来限制循环，但必须读取他

    string s; // s 用来存储第二行的数字串，例如 “0123456789”
    cin >> s; 
    // 外层是要输出每一行
    // 外层：显示屏一共有 5 行（0到4），这是打印一个图形
    for (int i = 0; i < 5; ++i) {
        // 内层遍历输入的数字串的每一位，打印的是不同的数
        // 输出每一行的各个数字部分
        for (int j = 0; j < n; ++j) {
            // 输出当前数字在当前（第i行）的点阵图案
            // num[d][i] 就是数字 d 的第 i 行字符串（长度为3）
            int d = s[j] - '0';
            cout << num[d][i];

            // 如果不是最后一个数字，则输出一个点 ‘.’ 作为列间隔
            if (j != n - 1) cout << '.';
        }
        // 每输出完显示屏的一整行，换行（'\n'更快，endl 会刷新缓冲区）
        cout << '\n';
    }
    return 0;
}