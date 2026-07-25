MAXN = 10010
tree = [True] * MAXN

l, m = map(int, input().split())

for i in range(m):
    u, v = map(int, input().split())
    for j in range(u, v + 1):
        tree[j] = False

count = sum(1 for i in range(l + 1) if tree[i])
print(count)
