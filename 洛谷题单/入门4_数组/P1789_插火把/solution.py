def main():
    n, m, k = map(int, input().split())
    
    # 初始化数组
    light = [[False] * (n + 1) for _ in range(n + 1)]
    item = [[False] * (n + 1) for _ in range(n + 1)]
    
    # 火把照明范围（相对坐标）
    torch_offsets = [
        (-2, 0), (-1, -1), (-1, 0), (-1, 1),
        (0, -2), (0, -1), (0, 0), (0, 1), (0, 2),
        (1, -1), (1, 0), (1, 1), (2, 0)
    ]
    
    # 萤石照明范围（相对坐标）
    glow_offsets = []
    for i in range(-2, 3):
        for j in range(-2, 3):
            glow_offsets.append((i, j))
    
    # 处理火把
    for _ in range(m):
        x, y = map(int, input().split())
        item[x][y] = True
        
        # 标记火把照亮的区域
        for dx, dy in torch_offsets:
            nx, ny = x + dx, y + dy
            if 1 <= nx <= n and 1 <= ny <= n:
                light[nx][ny] = True
    
    # 处理萤石
    for _ in range(k):
        x, y = map(int, input().split())
        item[x][y] = True
        
        # 标记萤石照亮的区域
        for dx, dy in glow_offsets:
            nx, ny = x + dx, y + dy
            if 1 <= nx <= n and 1 <= ny <= n:
                light[nx][ny] = True
    
    # 统计生成怪物的格子数量
    monster_count = 0
    for i in range(1, n + 1):
        for j in range(1, n + 1):
            if not light[i][j] and not item[i][j]:
                monster_count += 1
    
    print(monster_count)

if __name__ == "__main__":
    main()