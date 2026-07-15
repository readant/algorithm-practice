#include <iostream>
using namespace std;

int main(){
    // 优化输入输出速度
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n;
    cin >> n;
    
    // 定义二维数组存储杨辉三角，大小为25x25（满足n≤20）
    int a[25][25] = {0};
    
    // 构建杨辉三角
    for(int i = 0; i < n; ++i){
        // 杨辉三角性质：每行第一个元素都是1
        a[i][0] = 1;
        
        // 杨辉三角性质：每行最后一个元素都是1
        a[i][i] = 1;
        
        // 计算中间的元素（从第2个到倒数第2个）
        // 递推公式：a[i][j] = a[i-1][j-1] + a[i-1][j]
        for(int j = 1; j < i; ++j){
            a[i][j] = a[i-1][j-1] + a[i-1][j];
        }
    }

    // 输出杨辉三角
    for(int i = 0; i < n; ++i){
        // 输出第i行的前i+1个元素
        for(int j = 0; j <= i; ++j){
            // 数字间用空格分隔，第一个数字前不加空格
            if(j > 0) cout << " ";
            cout << a[i][j];
        }
        // 每行输出后换行
        cout << endl;
    }
    
    return 0;
}