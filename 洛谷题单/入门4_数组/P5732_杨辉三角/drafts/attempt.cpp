#include <iostream>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >> n;

    int a[25][25] = {0};
    int i,j;
    // 每一行的开头结尾都是1

    for(int i = 0;i<n;++i){
        a[i][0] = 1;
        a[i][i] = 1;
        for(int j=0;j<=i;++j){


            if(i>1){
                a[i][j] = a[i-1][j]+a[i-1][j-1];
            }
            if(j>0) cout << ' ';
            cout << a[i][j]; 

        }
        cout << '\n';
    }



    return 0;
}

// 20260714 第一次复现成功 用时30min
