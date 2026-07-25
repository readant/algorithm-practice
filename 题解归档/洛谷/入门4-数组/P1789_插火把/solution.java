import java.util.Scanner;

class Main {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int n = sc.nextInt(), m = sc.nextInt(), k = sc.nextInt();

        boolean[][] light = new boolean[n + 1][n + 1];
        boolean[][] item = new boolean[n + 1][n + 1];

        int[] torchDx = {-2,-1,-1,-1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
        int[] torchDy = { 0,-1, 0, 1,-2,-1, 0, 1, 2,-1, 0, 1, 0};

        for (int i = 0; i < m; i++) {
            int x = sc.nextInt(), y = sc.nextInt();
            item[x][y] = true;
            for (int j = 0; j < 13; j++) {
                int nx = x + torchDx[j], ny = y + torchDy[j];
                if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
                    light[nx][ny] = true;
            }
        }

        for (int i = 0; i < k; i++) {
            int x = sc.nextInt(), y = sc.nextInt();
            item[x][y] = true;
            for (int dx = -2; dx <= 2; dx++)
                for (int dy = -2; dy <= 2; dy++) {
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 1 && nx <= n && ny >= 1 && ny <= n)
                        light[nx][ny] = true;
                }
        }

        int ans = 0;
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= n; j++)
                if (!light[i][j] && !item[i][j])
                    ans++;

        System.out.println(ans);
        sc.close();
    }
}
