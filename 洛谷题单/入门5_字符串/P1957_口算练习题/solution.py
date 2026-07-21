T = int(input())
for _ in range(T):
    s = input()
    parts = s.replace('=', '+').split('+')
    a, b, c = int(parts[0]), int(parts[1]), int(parts[2])
    print("YES" if a + b == c else "NO")
