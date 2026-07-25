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
        
        int cnt = 0;
        for (int i = 0; i < 7; ++i) {
            for (int j = 0; j < 7; ++j) {
                if (prize[i] == bet[j]) {
                    cnt++;
                    break;
                }
            }
        }

        if (cnt == 7) cout << 1 << endl;
        else if (cnt == 6) cout << 2 << endl;
        else if (cnt == 5) cout << 3 << endl;
        else if (cnt == 4) cout << 4 << endl;
        else if (cnt == 3) cout << 5 << endl;
        else cout << 0 << endl;
    }
    
    return 0;
}