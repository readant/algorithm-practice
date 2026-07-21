import java.util.Scanner;

class Main {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int n = sc.nextInt();

        int[][] a = new int[25][25];

        for (int i = 0; i < n; ++i) {
            a[i][0] = 1;
            a[i][i] = 1;
            for (int j = 1; j < i; ++j) {
                a[i][j] = a[i - 1][j - 1] + a[i - 1][j];
            }
        }

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j <= i; ++j) {
                if (j > 0) {
                    System.out.print(" ");
                }
                System.out.print(a[i][j]);
            }
            System.out.println();
        }

        sc.close();
    }
}
