// 牛客周赛 Round 164 · E 小红的排列构造  （正解 / 逆向构造 + 隐式 treap）
//
// 题意：构造 1..n 的排列 p，对其做 n 次操作：第 i 次，
//   若当前长度 len 为奇数，记 idx=(len+1)/2，若 p[idx]==i 则计 1 分；
//   然后删除值为 i 的元素。要求恰好计 k 分，否则输出 No。
//
// 逆向构造：正向第 i 步删掉值 i。倒过来——从后往前，每次把值 i "插回"到当前序列。
//   第 i 步（正向）时，场上剩 {i, i+1, ..., n}，长度 L = n-i+1。
//   若该步要得分，就让值 i 落在当前序列正中间（0-indexed 位置 (L-1)/2）；否则放最前。
//   用隐式 treap 按位置插入，O(n log n)。
//
// 可达性：只有 L 为奇数时才可能得分，最多 ceil(n/2) 次，故 k > ceil(n/2) 必 No。

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

struct Node {
    int val, prio, sz;
    Node *l, *r;
    Node(int v) : val(v), prio(rand()), sz(1), l(nullptr), r(nullptr) {}
};
int sz(Node* t) { return t ? t->sz : 0; }
void upd(Node* t) { if (t) t->sz = 1 + sz(t->l) + sz(t->r); }

void split(Node* t, int k, Node*& L, Node*& R) {
    if (!t) { L = R = nullptr; return; }
    if (sz(t->l) < k) {
        split(t->r, k - sz(t->l) - 1, t->r, R);
        L = t;
    } else {
        split(t->l, k, L, t->l);
        R = t;
    }
    upd(t);
}
Node* merge(Node* L, Node* R) {
    if (!L || !R) return L ? L : R;
    if (L->prio > R->prio) { L->r = merge(L->r, R); upd(L); return L; }
    else { R->l = merge(L, R->l); upd(R); return R; }
}
void inorder(Node* t, vector<int>& out) {
    if (!t) return;
    inorder(t->l, out);
    out.push_back(t->val);
    inorder(t->r, out);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    srand((unsigned)time(nullptr));

    int n, k;
    if (!(cin >> n >> k)) return 0;

    Node* root = nullptr;
    int need = k;
    for (int i = n; i >= 1; i--) {
        int L = (n - i) + 1;     // 第 i 步（正向）时场上长度
        int p;                   // 把值 i 插入到当前序列的 0-indexed 位置
        if (L % 2 == 1 && need > 0) {
            p = (L - 1) / 2;     // 落在正中间 => 得分
            need--;
        } else {
            p = 0;               // 最前 => 不得分
        }
        Node *Lp, *Rp;
        split(root, p, Lp, Rp);
        root = merge(merge(Lp, new Node(i)), Rp);
    }

    if (need != 0) {
        cout << "No\n";
        return 0;
    }
    vector<int> p_arr;
    inorder(root, p_arr);
    cout << "Yes\n";
    for (int i = 0; i < n; i++) {
        if (i) cout << ' ';
        cout << p_arr[i];
    }
    cout << '\n';
    return 0;
}
