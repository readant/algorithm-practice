// 洛谷 P5707 上学迟到   （模拟 / 时间换算）
//
// 题意：8:00 前到校，走路速度 v 米/分钟、路程 s 米，出门还要垃圾分类 10 分钟。求最晚出发时刻。
//
// 全部换算成"从 0:00 起的分钟数"再算，最后除以 60 取整、取余得小时分钟。
// 走路分钟数向上取整：(s + v - 1) / v。

#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int s, v;
    cin >> s >> v;

    int walkTime = (s + v - 1) / v;                  // 向上取整
    int totalTime = walkTime + 10;                   // 加垃圾分类 10 分钟
    int startMinutes = 8 * 60 - totalTime;
    if (startMinutes < 0) startMinutes += 24 * 60;   // 跨天

    cout << setfill('0') << setw(2) << startMinutes / 60 << ':'
         << setw(2) << startMinutes % 60 << '\n';
    return 0;
}
