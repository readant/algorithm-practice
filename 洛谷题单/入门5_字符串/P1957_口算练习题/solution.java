import java.util.Scanner;

class Main {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int n = sc.nextInt();
        
        char op = 0;
        
        for (int i = 0; i < n; i++) {
            String s = sc.next();
            
            int a, b;
            char symbol;
            
            if (s.equals("a") || s.equals("b") || s.equals("c")) {
                if (s.equals("a")) { op = '+'; symbol = '+'; }
                else if (s.equals("b")) { op = '-'; symbol = '-'; }
                else { op = '*'; symbol = '*'; }
                a = sc.nextInt();
                b = sc.nextInt();
            } else {
                symbol = op;
                a = Integer.parseInt(s);
                b = sc.nextInt();
            }
            
            int result;
            if (symbol == '+') result = a + b;
            else if (symbol == '-') result = a - b;
            else result = a * b;
            
            String expr = a + "" + symbol + b + "=" + result;
            System.out.println(expr);
            System.out.println(expr.length());
        }
    }
}
