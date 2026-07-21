n = int(input())
s = input()
result = ""
for c in s:
    result += chr((ord(c) - ord('a') + n) % 26 + ord('a'))
print(result)
