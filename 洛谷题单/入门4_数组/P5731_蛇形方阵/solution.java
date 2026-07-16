import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int n = sc.nextInt();
        sc.close();

        int[][] a = new int[10][10];

        // 方向：右、下、左、上
        int[] dx = {0, 1, 0, -1};
        int[] dy = {1, 0, -1, 0};

        int x = 0, y = 0, dir = 0;

        for (int num = 1; num <= n * n; ++num) {
            a[x][y] = num;
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            // 越界或已填充则转向
            if (nx < 0 || nx >= n || ny < 0 || ny >= n || a[nx][ny] != 0) {
                dir = (dir + 1) % 4;
                nx = x + dx[dir];
                ny = y + dy[dir];
            }
            x = nx;
            y = ny;
        }

        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                sb.append(String.format("%3d", a[i][j]));
            }
            sb.append('\n');
        }
        System.out.print(sb);
    }
}
