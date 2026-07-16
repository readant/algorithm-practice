#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string s,t;
    while(cin >> t){
        s += t;
    }

    int N = sqrt(s.size());
    vector<int> ans = {N};
    if(s[0]=='1') ans.push_back(0);
    for(int i = 1,cnt = 1;i<=(int)s.size();++i){
        if(i == (int)s.size()||s[i] != s[i-1]){
            ans.push_back(cnt);
            cnt = 1;
        }
        else cnt++;
    }
    for(int i = 0; i <(int)ans.size();++i){
        if(i) cout << ' ';
        cout << ans[i];
    }
    return 0;
}