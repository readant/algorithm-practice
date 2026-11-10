#include <iostream>
using namespace std;

// MAX_VAL：允许输入数字的最大值（0 ~ MAX_VAL）
const int MAX_VAL = 1000;
// 数组长度 = 最大值 + 1，下标 0 ~ 1000
int bucket[MAX_VAL + 1] = {0};

// 1. 录入数据，构建桶
void inputData()
{
    int x;
    cout << "请输入若干整数(0~1000)，输入 -1 结束录入：" << endl;
    while (cin >> x)
    {
        if (x == -1) break;
        if (x < 0 || x > MAX_VAL)
        {
            cout << x << " 超出范围！跳过（会越界）" << endl;
            continue;
        }
        bucket[x]++;
    }
    cout << "录入完成\n" << endl;
}

// 2. 查询某个数字出现频次
void queryCount()
{
    int x;
    cout << "请输入要查询的数字：";
    cin >> x;
    if (x < 0 || x > MAX_VAL)
    {
        cout << "数字超出范围！" << endl;
        return;
    }
    cout << x << " 出现次数：" << bucket[x] << "\n" << endl;
}

// 3. 判断数字是否存在
void checkExist()
{
    int x;
    cout << "请输入要检查的数字：";
    cin >> x;
    if (x < 0 || x > MAX_VAL)
    {
        cout << "数字超出范围！" << endl;
        return;
    }
    if (bucket[x] > 0)
        cout << x << " 存在\n";
    else
        cout << x << " 不存在\n";
    cout << endl;
}

// 4. 去重输出（只打印出现过的数，升序）
void printUnique()
{
    cout << "去重结果：";
    for (int i = 0; i <= MAX_VAL; i++)
    {
        if (bucket[i] > 0)
        {
            cout << i << " ";
        }
    }
    cout << "\n" << endl;
}

// 5. 桶计数排序输出（保留重复，升序）
void sortPrint()
{
    cout << "排序结果：";
    for (int i = 0; i <= MAX_VAL; i++)
    {
        for (int j = 0; j < bucket[i]; j++)
        {
            cout << i << " ";
        }
    }
    cout << "\n" << endl;
}

int main()
{
    int op;
    while (true)
    {
        cout << "===== 桶计数演示菜单 =====" << endl;
        cout << "1. 录入数据" << endl;
        cout << "2. 查询数字频次" << endl;
        cout << "3. 判断数字是否存在" << endl;
        cout << "4. 去重输出" << endl;
        cout << "5. 升序排序输出" << endl;
        cout << "0. 退出程序" << endl;
        cout << "==========================" << endl;
        cout << "请选择操作：";
        cin >> op;

        if (op == 0)
        {
            cout << "程序结束" << endl;
            break;
        }
        switch (op)
        {
            case 1: inputData(); break;
            case 2: queryCount(); break;
            case 3: checkExist(); break;
            case 4: printUnique(); break;
            case 5: sortPrint(); break;
            default: cout << "输入选项无效！\n" << endl;
        }
    }
    return 0;
}
