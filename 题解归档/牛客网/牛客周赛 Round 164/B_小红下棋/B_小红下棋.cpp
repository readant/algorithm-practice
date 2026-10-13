#include <iostream>
using namespace std;

// 3*3棋盘
char c[3][3];

// 判断小红是否获胜
bool is_win()
{
    // 检查所有行
    for(int i = 0; i < 3; i++)
    {
        if(c[i][0] == '1' && c[i][1] == '1' && c[i][2] == '1')
            return true;
    }
    // 检查所有列
    for(int i = 0; i < 3; i++)
    {
        if(c[0][i] == '1' && c[1][i] == '1' && c[2][i] == '1')
            return true;
    }
    // 两条对角线
    if(c[0][0] == '1' && c[1][1] == '1' && c[2][2] == '1')
        return true;
    if(c[0][2] == '1' && c[1][1] == '1' && c[2][0] == '1')
        return true;
    return false;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 输入棋盘
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            cin >> c[i][j];
        }
    }

    // 枚举每一个空位尝试落子
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(c[i][j] == '*')
            {
                c[i][j] = '1';    // 模拟小红落子
                if(is_win())
                {
                    cout << "Yes\n";
                    cout << i+1 << " " << j+1 << "\n";
                    return 0;
                }
                c[i][j] = '*';    // 回溯，恢复为空位，继续尝试下一个位置
            }
        }
    }
    // 所有空位尝试完都不能获胜
    cout << "No\n";
    return 0;
}
