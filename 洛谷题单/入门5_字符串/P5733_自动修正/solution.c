#include <stdio.h>
#include <string.h>

int main() {
    char s[101];  // 题目说最长100，多开1位存 '\0'
    scanf("%s", s);
    
    int len = strlen(s);
    for (int i = 0; i < len; i++) {
        // 判断是否为小写字母（'a'~'z'）
        if (s[i] >= 'a' && s[i] <= 'z') {
            // 位运算：清除第6位，小写转大写（差值32）
            // 32 = 0010 0000，~32 = 1101 1111
            s[i] &= ~32;
        }
    }
    
    printf("%s\n", s);
    return 0;
}
