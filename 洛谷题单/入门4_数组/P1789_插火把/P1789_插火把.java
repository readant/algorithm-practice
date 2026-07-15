import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        int n = scanner.nextInt();
        int m = scanner.nextInt();
        int k = scanner.nextInt();
        
        // 初始化数组
        boolean[][] light = new boolean[n + 1][n + 1];
        boolean[][] item = new boolean[n + 1][n + 1];
        
        // 火把照明范围（相对坐标）
        int[] torchDx = {-2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 2};
        int[] torchDy = {0, -1, 0, 1, -2, -1, 0, 1, 2, -1, 0, 1, 0};
        
        // 萤石照明范围（相对坐标）
        int[] glowDx = new int[25];
        int[] glowDy = new int[25];
        int index = 0;
        for (int i = -2; i <= 2; i++) {
            for (int j = -2; j <= 2; j++) {
                glowDx[index] = i;
                glowDy[index] = j;
                index++;
            }
        }
        
        // 处理火把
        for (int i = 0; i < m; i++) {
            int x = scanner.nextInt();
            int y = scanner.nextInt();
            item[x][y] = true;
            
            // 标记火把照亮的区域
            for (int j = 0; j < torchDx.length; j++) {
                int nx = x + torchDx[j];
                int ny = y + torchDy[j];
                if (nx >= 1 && nx <= n && ny >= 1 && ny <= n) {
                    light[nx][ny] = true;
                }
            }
        }
        
        // 处理萤石
        for (int i = 0; i < k; i++) {
            int x = scanner.nextInt();
            int y = scanner.nextInt();
            item[x][y] = true;
            
            // 标记萤石照亮的区域
            for (int j = 0; j < glowDx.length; j++) {
                int nx = x + glowDx[j];
                int ny = y + glowDy[j];
                if (nx >= 1 && nx <= n && ny >= 1 && ny <= n) {
                    light[nx][ny] = true;
                }
            }
        }
        
        // 统计生成怪物的格子数量
        int monsterCount = 0;
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (!light[i][j] && !item[i][j]) {
                    monsterCount++;
                }
            }
        }
        
        System.out.println(monsterCount);
        scanner.close();
    }
}