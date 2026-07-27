#include <iostream>   // 输入输出流
#include <vector>   // 向量容器
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int x, y;
    cin >> x >> y;

    vector<int> ans;
    int cnt = 0;

    for (int i = x; i <= y; ++i) {
        if ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0) {
            cnt++;
            ans.push_back(i);
        }
    }

    cout << cnt << '\n';
    for (size_t i = 0; i < ans.size(); ++i) {
        cout << ans[i] << (i + 1 == ans.size() ? '\n' : ' ');
    }

    return 0;
}