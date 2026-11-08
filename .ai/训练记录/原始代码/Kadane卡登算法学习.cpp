// Kadane 卡登算法：复用前面已经算好的结果，不要重复计算

#include<iostream>
using namespace std;

int n,a,b,i,ans = -2147483647;

int main(){
    cin >> n;
    for(i = 1;i <= n;i++){
        cin>>a;
        if(i == 1) b = a;
        else b = max(a,a+b);
        ans = max(ans,b);
    }
    cout << ans;
    return 0;
}
