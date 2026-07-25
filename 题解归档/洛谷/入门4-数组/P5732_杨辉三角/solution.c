#include <stdio.h>

int main(){
    int n;
    scanf("%d", &n);  // 读取输入n
    
    // 定义二维数组，C语言需要静态分配内存
    int a[25][25] = {0};  // 初始化为0
    
    // 构建杨辉三角
    for(int i = 0; i < n; ++i){
        a[i][0] = 1;    // 每行第一个元素
        a[i][i] = 1;    // 每行最后一个元素
        for(int j = 1; j < i; ++j){
            // 递推公式：当前元素 = 上方左 + 上方右
            a[i][j] = a[i-1][j-1] + a[i-1][j];
        }
    }
    
    // 输出杨辉三角
    for(int i = 0; i < n; ++i){
        for(int j = 0; j <= i; ++j){
            if(j > 0) printf(" ");  // 空格分隔
            printf("%d", a[i][j]);   // 输出数字
        }
        printf("\n");  // 换行
    }
    
    return 0;
}