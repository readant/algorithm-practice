n, m, k = map(int, input().split())

light = [[False] * (n + 1) for _ in range(n + 1)]
item = [[False] * (n + 1) for _ in range(n + 1)]

# 火把照明范围（13格）
torch_offsets = [
    (-2, 0), (-1, -1), (-1, 0), (-1, 1),
    (0, -2), (0, -1), (0, 0), (0, 1), (0, 2),
    (1, -1), (1, 0), (1, 1), (2, 0)
]

for _ in range(m):
    x, y = map(int, input().split())
    item[x][y] = True
    for dx, dy in torch_offsets:
        nx, ny = x + dx, y + dy
        if 1 <= nx <= n and 1 <= ny <= n:
            light[nx][ny] = True

for _ in range(k):
    x, y = map(int, input().split())
    item[x][y] = True
    for dx in range(-2, 3):
        for dy in range(-2, 3):
            nx, ny = x + dx, y + dy
            if 1 <= nx <= n and 1 <= ny <= n:
                light[nx][ny] = True

ans = 0
for i in range(1, n + 1):
    for j in range(1, n + 1):
        if not light[i][j] and not item[i][j]:
            ans += 1

print(ans)
