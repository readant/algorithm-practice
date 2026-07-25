import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        String s = sc.next();  // 读入一个字符串（遇空格停止）
        
        // toUpperCase() 返回大写版本，原字符串不变
        System.out.println(s.toUpperCase());
    }
}
