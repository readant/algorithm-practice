// 牛客周赛 Round 169 · B 小红的字符串压缩（第一次提交：格式错误）
// 赛内得分：60 / 100（40 个测试点里 15 个「格式错误」，24 个得 2.50 分）
// 错因：并列最多的字符有多个时，题面要求用空格分隔，这里连着输出了。
//       恰好只有一个字符并列最多的数据点不受影响，所以过了 24 个点。
// 赛内写法原样保留，未做任何修改。
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
            cnt[now]++;
            add(len);
            // 开启下一个
            now = s[i];
            len = 1;
        }
    }
    cnt[now]++;
    add(len);

    for(int i = 0;i < 256;i++) {
        maxc = max(maxc,cnt[i]);
    }

    for(int i = 0;i < 256;i++) {
        if(cnt[i] == maxc) {
            cout << (char)i;
        }
    }
    cout << '\n';
    return 0;
}
