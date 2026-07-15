
import java.util.Scanner;

class Main {

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int n = sc.nextInt();  // 读取输入n

        // Java使用二维数组，需要指定大小
        int[][] a = new int[25][25];

        // 构建杨辉三角
        for (int i = 0; i < n; ++i) {
            a[i][0] = 1;    // 每行第一个元素
            a[i][i] = 1;    // 每行最后一个元素
            for (int j = 1; j < i; ++j) {
                // 递推公式：当前元素 = 上方左 + 上方右
                a[i][j] = a[i - 1][j - 1] + a[i - 1][j];
            }
        }

        // 输出杨辉三角
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j <= i; ++j) {
                if (j > 0) {
                    System.out.print(" ");  // 空格分隔
                }
                System.out.print(a[i][j]);  // 输出数字
            }
            System.out.println();  // 换行
        }

        sc.close();  // 关闭扫描器
    }
}
