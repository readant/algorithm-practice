#include <iostream>
using namespace std;

int main() {
    int prize[7];
    for (int i = 0; i < 7; ++i) {
        cin >> prize[i];
    }
    
    int n;
    cin >> n;
    
    while (n--) {
        int bet[7];
        for (int i = 0; i < 7; ++i) {
            cin >> bet[i];
        }
        
        int count = 0;
        for (int i = 0; i < 7; ++i) {
            for (int j = 0; j < 7; ++j) {
                if (prize[i] == bet[j]) {
                    count++;
                    break;
                }
            }
        }
        
        if (count == 7) cout << 1 << endl;
        else if (count == 6) cout << 2 << endl;
        else if (count == 5) cout << 3 << endl;
        else if (count == 4) cout << 4 << endl;
        else if (count == 3) cout << 5 << endl;
        else cout << 0 << endl;
    }
    
    return 0;
}