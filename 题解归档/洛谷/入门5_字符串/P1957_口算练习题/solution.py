n = int(input())
op = ''

for _ in range(n):
    parts = input().split()
    
    if len(parts) == 3:
        if parts[0] == 'a': op = '+'
        elif parts[0] == 'b': op = '-'
        else: op = '*'
        a, b = int(parts[1]), int(parts[2])
    else:
        a, b = int(parts[0]), int(parts[1])
    
    if op == '+': result = a + b
    elif op == '-': result = a - b
    else: result = a * b
    
    expr = f"{a}{op}{b}={result}"
    print(expr)
    print(len(expr))
