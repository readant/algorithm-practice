/*	顺序表插入
在第 i 位插入 e，需把第 i ~ n 位元素整体后移一格。
必须从最后一个元素开始、从后往前挪，否则会覆盖。

*/


#include <iostream>
// 预定义常量及类型
#define OK 1
#define ERROR 0
#define OVERFLOW -2
// 顺序表可能达到的最大长度
#define MAXSIZE 100
// 自定义元素类型，如我在这里举例 int
typedef int ElemType;
typedef int Status;
using namespace std;

typedef struct {
	ElemType *elem;
	int length;
} SqList;

// 初始化函数
Status InitList(SqList &L) {
	L.elem = new ElemType[MAXSIZE];
	if (!L.elem)
		return OVERFLOW;
	L.length = 0;
	return OK;
}

//
Status ListInsert(SqList &L, int i, ElemType e) {
	if ((i < 1) || (i > L.length + 1)) return ERROR;
	if (L.length == MAXSIZE) return ERROR;
	for (int j = L.length - 1; j >= i - 1; j--)
		L.elem[j + 1] = L.elem[j];
	L.elem[i - 1] = e;
	++ L.length;
	return OK;

}

// 销毁函数
void DestroyList(SqList &L) {
	delete[] L.elem;
	L.elem = nullptr;
	L.length = 0;
}

// main函数：程序入口，必须要有!
int main() {
	SqList L;

	DestroyList(L);
	return 0;
}
