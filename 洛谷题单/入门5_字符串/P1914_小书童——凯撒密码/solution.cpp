#include <iostream>
#include <string>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    string s;
    cin >> n >> s;
    for(int i = 0;i < s.size();++i){
        s[i] = (s[i] - 'a' + n)  % 26 + 'a';
    }
    cout << s;
    return 0;
}