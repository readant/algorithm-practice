#include <iostream>
using namespace std;

const int MAXN = 10010;
// 这里只会让第一个元素变成 true,其他的还是false,单独拿出来展示一下
bool tree[MAXN] = {true};

int main() {
    int l, m;
    cin >> l >> m;
    // bool数组全局定义时默认为false,需要手动初始化
    for (int i = 0; i <= l; ++i)
        tree[i] = true;

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        for (int j = u; j <= v; ++j) {
            tree[j] = false;
        }
    }

    int cnt = 0;
    for (int i = 0; i <= l; ++i) {
        if (tree[i]) cnt++;
    }

    cout << cnt << '\n';
    return 0;
}