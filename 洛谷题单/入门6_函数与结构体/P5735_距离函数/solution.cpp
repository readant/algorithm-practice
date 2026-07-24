#include <iostream>
#include <cmath>     // sqrt 和 pow 函数在这里
#include <iomanip>   // setprecision 在这里
using namespace std;

int main() {
    // 第1步：定义6个变量，存三个点的坐标
    double x1, y1, x2, y2, x3, y3;
    cin >> x1 >> y1;   // 第一个点 (x1, y1)
    cin >> x2 >> y2;   // 第二个点 (x2, y2)
    cin >> x3 >> y3;   // 第三个点 (x3, y3)

    // 第2步：用公式算三条边的长度
    // 两点距离公式：d = sqrt((x2-x1)² + (y2-y1)²)
    double d12 = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));  // A到B
    double d23 = sqrt(pow(x3 - x2, 2) + pow(y3 - y2, 2));  // B到C
    double d31 = sqrt(pow(x1 - x3, 2) + pow(y1 - y3, 2));  // C到A

    // 第3步：三条边加起来 = 周长
    double perimeter = d12 + d23 + d31;

    // 第4步：保留两位小数输出
    cout << fixed << setprecision(2) << perimeter << endl;

    return 0;
}