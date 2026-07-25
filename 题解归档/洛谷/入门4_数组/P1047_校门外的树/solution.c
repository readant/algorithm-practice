#include <stdio.h>

#define MAXN 10010

int tree[MAXN];

int main() {
    int l, m;
    scanf("%d %d", &l, &m);

    for (int i = 0; i <= l; i++)
        tree[i] = 1;

    for (int i = 0; i < m; i++) {
        int u, v;
        scanf("%d %d", &u, &v);
        for (int j = u; j <= v; j++)
            tree[j] = 0;
    }

    int cnt = 0;
    for (int i = 0; i <= l; i++) {
        if (tree[i]) cnt++;
    }

    printf("%d\n", cnt);
    return 0;
}
