#include <iostream>
#include <vector> //其实这道题有明确约定上限，就可以完全使用传统的数组

using namespace std;

int main()
{
    int n, m;

    cin >> n >> m;
    // 没仔细看题目条件，这是一个很不好的习惯,导致数组严重溢出
    // vector<int> a(101);
    vector<int> a(n);
    for (int j = 0; j < n; ++j)
    {
        cin >> a[j];
    }
    int sum = 0;

    // 卡顿处：
    // 通过遍历进行实现统计,如何才能实现求得连续M个刺痛值的和的统计
    for (int i = 0; i < n; ++i)
    {
        // 以下就是一点也没有写的地方了
        for(int j = 0;j<i;++j){
            sum += a[i];
        }
        int min = sum;
        for(int i = m; i <n;++i){
            sum = sum -a[i - m]+a[i];
            min = sum;
        }
    cout << min << endl;
    }
    return 0;
}
