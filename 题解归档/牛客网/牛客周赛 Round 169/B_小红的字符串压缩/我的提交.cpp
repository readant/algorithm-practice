// 牛客周赛 Round 169 · B 小红的字符串压缩（AC 版）
// 赛内得分：100 / 100（第 2 次提交，19:44:07），距首次提交 6 分 04 秒
// 与上一版的差异只有两处：
//   ① 三个 cnt[now]++ 改为 cnt[(int)now]++（消除 char 下标歧义，非错因）
//   ② 输出循环加了空格分隔（if(!first) cout << ' ';）——这才是格式错误的真正修复
// 赛内写法原样转录自牛客提交页，仅缩进可能有出入。
#include <bits/stdc++.h>

using namespace std;

int cnt[256] = {0};

void add(int num) {
    string t = to_string(num);
    for(char c: t) {
        cnt[c]++;
    }
}

int main() {
    int n;

    cin >> n;
    string s;
    cin >> s;
    char now = s[0];
    int len = 1;
    int maxc = 0;
    for(int i = 1;i<n;i++) {
        // 相同
        if(s[i] == now) len++;
        // 不同，切换
        else {
            // 记录上一个
            cnt[(int)now]++;
            add(len);
            // 开启下一个
            now = s[i];
            len = 1;
        }
    }

    cnt[(int)now]++;
    add(len);

    for(int i = 0;i < 256;i++) {
        maxc = max(maxc,cnt[i]);
    }

    bool first = true;
    for(int i = 0;i < 256;i++) {
        if(cnt[i] == maxc) {
            if(!first) cout << ' ';
            cout << (char)i;
            first = false;
        }
    }

    cout << '\n';
    return 0;
}
