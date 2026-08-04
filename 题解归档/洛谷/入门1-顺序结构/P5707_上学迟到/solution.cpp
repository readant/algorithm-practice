#include <cstdio>

using namespace std;

int main() {
    // 关闭 C++ 标准流与 C 标准流的同步，提高 cin/cout 速度
    // 但本题我们使用 printf，所以这一行不是必须的，不过保留也无妨
    // 我们选择使用 printf 来处理格式化，所以可以不开启同步
    // 为了代码清晰，这里只使用 scanf 和 printf，不混用 cin/cout，避免同步问题。
    
    int s, v;  // s: 路程（米），v: 速度（米/分钟）
    scanf("%d%d", &s, &v);  // 读入两个正整数
    
    // 计算步行所需分钟数，向上取整
    // 公式 (s + v - 1) / v 是整数向上取整的经典写法
    // 原理：如果 s 能被 v 整除，则 (s+v-1)/v = s/v；否则多出余数，加 v-1 后商进1
    int walkTime = (s + v - 1) / v;
    
    // 加上垃圾分类的 10 分钟
    int totalTime = walkTime + 10;
    
    // 8:00 对应的分钟数：8 * 60 = 480
    int limitMinutes = 8 * 60;
    
    // 最晚出发分钟数（从 0:00 开始计算的分钟数）
    int startMinutes = limitMinutes - totalTime;
    
    // 如果出发时间早于 0:00，说明要提前一天出发，加 24 小时（1440 分钟）
    if (startMinutes < 0) {
        startMinutes += 24 * 60;  // 跨天处理，题目保证提前不超过一天
    }
    
    // 计算小时和分钟
    int hour = startMinutes / 60;   // 整除得到小时
    int minute = startMinutes % 60; // 取余得到分钟
    
    // 使用 printf 格式化输出，%02d 表示输出两位整数，不足补前导零
    printf("%02d:%02d\n", hour, minute);
    // 换行使用 '\n'，不用 endl，避免强制刷新缓冲区
    
    return 0;
}