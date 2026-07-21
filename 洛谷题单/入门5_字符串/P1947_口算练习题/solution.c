#include <stdio.h>
#include <string.h>

int main() {
    int T;
    scanf("%d", &T);
    
    while (T--) {
        char s[50];
        scanf("%s", s);
        
        long long a = 0, b = 0, c = 0;
        int i = 0;
        
        while (s[i] != '+') {
            a = a * 10 + (s[i] - '0');
            i++;
        }
        i++;
        
        while (s[i] != '=') {
            b = b * 10 + (s[i] - '0');
            i++;
        }
        i++;
        
        while (s[i] != '\0') {
            c = c * 10 + (s[i] - '0');
            i++;
        }
        
        printf("%s\n", (a + b == c) ? "YES" : "NO");
    }
    
    return 0;
}
