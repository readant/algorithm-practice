n = int(input())  # 读取输入n

# Python使用列表嵌套，无需预先指定大小
a = [[0] * 25 for _ in range(25)]

# 构建杨辉三角
for i in range(n):
    a[i][0] = 1    # 每行第一个元素
    a[i][i] = 1    # 每行最后一个元素
    for j in range(1, i):
        # 递推公式：当前元素 = 上方左 + 上方右
        a[i][j] = a[i-1][j-1] + a[i-1][j]

# 输出杨辉三角
for i in range(n):
    # 使用join方法格式化输出，数字转字符串后用空格连接
    print(' '.join(str(a[i][j]) for j in range(i+1)))