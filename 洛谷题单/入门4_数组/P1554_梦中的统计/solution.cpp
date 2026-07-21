
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int M, N;
    cin >> M >> N;

    // 用一个长度为10的数组记录数码0~9的出现次数，初始化为0
    vector<int> cnt(10, 0);   // 或者 int cnt[10] = {0};

    // 遍历区间内的每一个整数
    for (int i = M; i <= N; ++i) {
        int x = i;
        // 拆分x的每一位
        while (x > 0) {
            int d = x % 10;   // 取出个位数字
            cnt[d]++;         // 对应数码计数加1
            x /= 10;          // 去掉个位
        }
        // 注意：当i == 0时，while循环不会执行，需要单独处理
        // 但题目M≥1，所以可以忽略；如果M可能为0，则加上：
        // if (i == 0) cnt[0]++;
    }

    // 输出结果，用空格分隔
    for (int i = 0; i < 10; ++i) {
        cout << cnt[i];
        if (i != 9) cout << ' ';
    }
    cout << endl;

    return 0;
}