#include <iostream>
using namespace std;

// 预定义常量及类型
#define OK 1
#define ERROR 0
#define OVERFLOW -2
// 顺序表可能达到的最大长度
#define MAXSIZE 100
// 自定义元素类型，如我在这里举例 int
typedef int ElemType;
typedef int Status;

typedef struct 
{
	ElemType *elem;
	int length;
}SqList;

// 初始化函数
Status InitList(SqList &L)
{
	L.elem = new ElemType[MAXSIZE];
	if (!L.elem)
		return OVERFLOW;
	L.length = 0;
	return OK;
}
// 销毁函数
void DestroyList(SqList &L)
{
	delete[] L.elem;
	L.elem = nullptr;
	L.length = 0;
}

// main函数：程序入口，必须要有!
int main() 
{
	SqList L;
	if(InitList(L)==OK)
	{
		cout <<"顺序表初始化成功！" <<endl;
		cout <<"当前表长：" << L.length <<endl;
	}
	DestroyList(L);
	return 0;
}
