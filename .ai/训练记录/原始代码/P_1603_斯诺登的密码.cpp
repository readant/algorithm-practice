#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;

// 打表：dic[i] 是英文数字词，di[i] 是该词的对应值平方后 %100 的结果
// 提前算好，运行时直接查，省掉 v*v%100 那一步
const char dic[27][20] = {
    "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
    "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen",
    "seventeen", "eighteen", "nineteen", "twenty",
    "a", "both", "another", "first", "second", "third"
};
const int di[27] = {
    0, 1, 4, 9, 16, 25, 36, 49, 64, 81,
    0, 21, 44, 69, 96, 25, 56, 89, 24, 61,
    1, 4, 1, 1, 4, 9
};

int nums[10];   // 最多 6 个数字词，开 10 够用
int top = 0;

int main() {
    char s[100];
    for (int k = 0; k < 6; ++k) {
        scanf("%s", s);   // 一次读一个单词，自动跳过空格

        // 统一转小写，并截断非字母字符（如粘在词尾的句号）
        for (int i = 0; s[i]; ++i) {
            if (s[i] >= 'A' && s[i] <= 'Z') {
                s[i] = s[i] - 'A' + 'a';
            } else if (!(s[i] >= 'a' && s[i] <= 'z')) {
                s[i] = '\0';   // 从非字母处截断
                break;
            }
        }

        // 在表里查这个词
        for (int j = 0; j < 27; ++j) {
            if (strcmp(s, dic[j]) == 0) {
                nums[top++] = di[j];
                break;
            }
        }
    }

    sort(nums, nums + top);   // 每个数固定两位，升序拼接即最小

    bool started = false;   // 是否已经输出过第一个非零块
    for (int i = 0; i < top; ++i) {
        if (started) {
            printf("%02d", nums[i]);       // 后续一律补两位，如 4 -> "04"
        } else if (nums[i]) {
            printf("%d", nums[i]);         // 第一个非零块：原样输出，去掉前导零
            started = true;
        }
    }
    if (!started) printf("0");             // 全是 0 或没有任何数字词
    printf("\n");

    return 0;
}