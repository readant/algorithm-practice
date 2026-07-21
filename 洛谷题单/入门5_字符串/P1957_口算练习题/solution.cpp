#include <iostream>
#include <string>
using namespace std;

int main() {
    int n;
    cin >> n;
    
    char op = 0;
    
    for (int i = 0; i < n; i++) {
        string s;
        cin >> s;
        
        int a, b, result;
        char symbol;
        
        if (s == "a" || s == "b" || s == "c") {
            if (s == "a") { op = '+'; symbol = '+'; }
            else if (s == "b") { op = '-'; symbol = '-'; }
            else { op = '*'; symbol = '*'; }
            cin >> a >> b;
        } else {
            symbol = op;
            a = stoi(s);
            cin >> b;
        }
        
        if (symbol == '+') result = a + b;
        else if (symbol == '-') result = a - b;
        else result = a * b;
        
        string expr = to_string(a) + symbol + to_string(b) + "=" + to_string(result);
        cout << expr << "\n" << expr.length() << "\n";
    }
    
    return 0;
}
