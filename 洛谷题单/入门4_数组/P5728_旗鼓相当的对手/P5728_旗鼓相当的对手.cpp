#include <iostream>
using namespace std;

int main() {
    int n;
    cin >> n;
    
    int scores[n][3];
    for (int i = 0; i < n; ++i) {
        cin >> scores[i][0] >> scores[i][1] >> scores[i][2];
    }
    
    int count = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int sum1 = scores[i][0] + scores[i][1] + scores[i][2];
            int sum2 = scores[j][0] + scores[j][1] + scores[j][2];
            
            if (abs(scores[i][0] - scores[j][0]) <= 5 && 
                abs(scores[i][1] - scores[j][1]) <= 5 && 
                abs(scores[i][2] - scores[j][2]) <= 5 && 
                abs(sum1 - sum2) <= 10) {
                count++;
            }
        }
    }
    
    cout << count << endl;
    return 0;
}