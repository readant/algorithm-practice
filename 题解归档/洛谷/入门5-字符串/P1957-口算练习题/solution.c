#include <stdio.h>
#include <string.h>

int main() {
    int n;
    scanf("%d", &n);
    
    char op = 0;
    
    for (int i = 0; i < n; i++) {
        char s[20];
        scanf("%s", s);
        
        int a, b, result;
        char symbol;
        
        if (strcmp(s, "a") == 0 || strcmp(s, "b") == 0 || strcmp(s, "c") == 0) {
            if (s[0] == 'a') { op = '+'; symbol = '+'; }
            else if (s[0] == 'b') { op = '-'; symbol = '-'; }
            else { op = '*'; symbol = '*'; }
            scanf("%d %d", &a, &b);
        } else {
            symbol = op;
            a = atoi(s);
            scanf("%d", &b);
        }
        
        if (symbol == '+') result = a + b;
        else if (symbol == '-') result = a - b;
        else result = a * b;
        
        char expr[50];
        sprintf(expr, "%d%c%d=%d", a, symbol, b, result);
        printf("%s\n%d\n", expr, (int)strlen(expr));
    }
    
    return 0;
}
