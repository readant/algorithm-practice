#include <iostream>
#include <string>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int T;
    cin >> T;
    
    while (T--) {
        string s;
        cin >> s;
        
        int plusPos = s.find('+');
        int equalPos = s.find('=');
        
        long long a = stoll(s.substr(0, plusPos));
        long long b = stoll(s.substr(plusPos + 1, equalPos - plusPos - 1));
        long long c = stoll(s.substr(equalPos + 1));
        
        cout << (a + b == c ? "YES" : "NO") << "\n";
    }
    
    return 0;
}
