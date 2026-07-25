import java.util.Scanner;

public class Main {
    static final int MAXN = 10010;
    static boolean[] tree = new boolean[MAXN];

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int l = sc.nextInt();
        int m = sc.nextInt();

        for (int i = 0; i <= l; i++)
            tree[i] = true;

        for (int i = 0; i < m; i++) {
            int u = sc.nextInt();
            int v = sc.nextInt();
            for (int j = u; j <= v; j++)
                tree[j] = false;
        }

        int cnt = 0;
        for (int i = 0; i <= l; i++) {
            if (tree[i]) cnt++;
        }

        System.out.println(cnt);
    }
}
