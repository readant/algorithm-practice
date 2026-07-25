n = int(input())
a = [[0] * n for _ in range(n)]

# 方向：右、下、左、上
dx = [0, 1, 0, -1]
dy = [1, 0, -1, 0]

x, y, d = 0, 0, 0

for num in range(1, n * n + 1):
    a[x][y] = num
    nx, ny = x + dx[d], y + dy[d]
    if nx < 0 or nx >= n or ny < 0 or ny >= n or a[nx][ny] != 0:
        d = (d + 1) % 4
        nx, ny = x + dx[d], y + dy[d]
    x, y = nx, ny

for row in a:
    print("".join(f"{v:3d}" for v in row))
