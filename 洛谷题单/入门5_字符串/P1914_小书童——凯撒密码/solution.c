#include <stdio.h>
#include <string.h>

int main() {
    int n;
    char s[51];
    scanf("%d %s", &n, s);
    
    for (int i = 0; i < strlen(s); i++) {
        s[i] = (s[i] - 'a' + n) % 26 + 'a';
    }
    
    printf("%s", s);
    return 0;
}
