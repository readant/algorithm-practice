#include <iostream>
#include <vector>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    
    int N;
    cin >> N;
    
    vector<vector<int>> magic(N, vector<int>(N, 0));
    int x = 0, y = N / 2;
    magic[x][y] = 1;
    
    for (int k = 2; k <= N * N; ++k) {
        int nx, ny;
        
        if (x == 0 && y != N - 1) {
            nx = N - 1;
            ny = y + 1;
        } else if (y == N - 1 && x != 0) {
            nx = x - 1;
            ny = 0;
        } else if (x == 0 && y == N - 1) {
            nx = x + 1;
            ny = y;
        } else {
            if (magic[x - 1][y + 1] == 0) {
                nx = x - 1;
                ny = y + 1;
            } else {
                nx = x + 1;
                ny = y;
            }
        }
        
        magic[nx][ny] = k;
        x = nx;
        y = ny;
    }
    
    for (const auto& row : magic) {
        for (size_t j = 0; j < row.size(); ++j) {
            cout << row[j];
            if (j + 1 < row.size()) cout << ' ';
        }
        cout << '\n';
    }
    
    return 0;
}