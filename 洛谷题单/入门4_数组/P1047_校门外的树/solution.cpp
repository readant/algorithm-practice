#include <iostream>
using namespace std;

const int MAXN = 10010;
bool tree[MAXN] = {true};

int main() {
    int L, M;
    cin >> L >> M;
    
    for (int i = 0; i < M; ++i) {
        int start, end;
        cin >> start >> end;
        for (int j = start; j <= end; ++j) {
            tree[j] = false;
        }
    }
    
    int count = 0;
    for (int i = 0; i <= L; ++i) {
        if (tree[i]) count++;
    }
    
    cout << count << endl;
    return 0;
}