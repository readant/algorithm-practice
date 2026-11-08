// 牛客周赛 Round 166 · D 题 · 小月的回声
// 赛内得分：60 / 200（提交于 00:43:53）
// 说明：本人赛内提交代码，原样归档，未做任何修改。
//
// 错在哪：三重循环 O(n^3)——枚举左右端点后再逐元素求中间和。
//        题面 n ≤ 2×10^5，允许的复杂度只有 O(n) / O(n log n)，
//        O(n^3) 注定只过小数据（60 分来自最小的一档测试点）。
// 正解见同目录 D_小月的回声.cpp：由 a_i ≥ 1 推出区间长度上界 ≤ 901，枚举 O(900n)。

#include <iostream>
using namespace std;
int a[100001];
int main() {
ios::sync_with_stdio(0);
cin.tie(0);
int n;
cin >> n;
for(int i = 1;i <= n;i++) {
cin >> a[i];
}
int cnt = 0;
// l
for(int i = 1;i<=n;i++) {
// r
for(int j = i;j <=n;j++){
int t = j - i + 1;
if(t >= 3){
int sum = 0;
for(int z = i + 1;z<j;z++) {
sum += a[z];
}
if(sum == a[i] * a[j]) cnt ++;
}
}
}
cout << cnt << "\n";
return 0;
}
