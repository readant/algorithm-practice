#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

int n;
vector<string> a, b; // a: 原始图案, b: 目标图案

// ---------- 辅助变换函数 ----------

// 1. 顺时针旋转 90°
vector<string> rotate90(const vector<string>& g) {
    vector<string> res(n, string(n, ' '));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            res[j][n - 1 - i] = g[i][j];
        }
    }
    return res;
}

// 2. 顺时针旋转 180° (执行两次90°)
vector<string> rotate180(const vector<string>& g) {
    return rotate90(rotate90(g));
}

// 3. 顺时针旋转 270° (执行三次90°)
vector<string> rotate270(const vector<string>& g) {
    return rotate90(rotate180(g));
}

// 4. 水平翻转（左右镜像）
vector<string> reflect(const vector<string>& g) {
    vector<string> res = g;
    for (int i = 0; i < n; i++) {
        reverse(res[i].begin(), res[i].end());
    }
    return res;
}

// 判断两个矩阵是否相等
bool same(const vector<string>& x, const vector<string>& y) {
    return x == y;
}

// ---------- 主程序 ----------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n;
    a.resize(n);
    b.resize(n);

    for (int i = 0; i < n; i++) cin >> a[i];
    for (int i = 0; i < n; i++) cin >> b[i];

    // 1. 旋转 90°
    if (same(rotate90(a), b)) {
        cout << 1 << '\n';
        return 0;
    }

    // 2. 旋转 180°
    if (same(rotate180(a), b)) {
        cout << 2 << '\n';
        return 0;
    }

    // 3. 旋转 270°
    if (same(rotate270(a), b)) {
        cout << 3 << '\n';
        return 0;
    }

    // 4. 水平翻转
    vector<string> reflected = reflect(a);
    if (same(reflected, b)) {
        cout << 4 << '\n';
        return 0;
    }

    // 5. 组合：水平翻转 + 旋转 90°/180°/270°
    if (same(rotate90(reflected), b) || 
        same(rotate180(reflected), b) || 
        same(rotate270(reflected), b)) {
        cout << 5 << '\n';
        return 0;
    }

    // 6. 不改变
    if (same(a, b)) {
        cout << 6 << '\n';
        return 0;
    }

    // 7. 无效转换
    cout << 7 << '\n';
    return 0;
}