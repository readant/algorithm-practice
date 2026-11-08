// 牛客周赛 Round 168 · A 小月的数组
// 赛内得分：50 / 50（第 4 次提交 AC，19:08:40；19:09:01 又交了一次同分）
// 提交记录：19:05:22 段错误 25 分 / 19:06:35 段错误 25 分 / 19:06:47 段错误 0 分 / 19:08:40 答案正确 50 分
// 当时错在哪：数组开在 main 里的栈上且尺度过大（本文件 a[10000001] = 80MB），前三次段错误；
//               换到更严格的评测机上同样会 RE。题目约束 n ≤ 2×10^5，只需 2n ≤ 4×10^5 的容量。
#include <iostream>

using namespace std;

int main() {
    int n;
    cin >> n;
    long long a[10000001];
    for(int i = 0;i<n+n;i++) {
        cin >> a[i];
    }

    for(int i = 0;i<n + n;i+=2) {
        cout << a[i] << ' ';
    }
    cout << endl;
    for(int i = 1;i<n + n;i+=2) {
        cout << a[i] <<' ';
    }
    cout << endl;
    return 0;
}
