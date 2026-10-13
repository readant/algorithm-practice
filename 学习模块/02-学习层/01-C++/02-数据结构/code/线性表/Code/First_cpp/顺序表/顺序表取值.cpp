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

// 按位取值核心算法
Status GetElem(SqList L, int i, ElemType &e)
{// 取第 i 个元素值，通过 e 返回
	if(i < 1 || i > L.length) 
		return ERROR;  // 越界检查
	e = L.elem[i-1];                         // 随机存取
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

	InitList(L);
	
	// 手动添加测试数据
	L.elem[0] = 11;
	L.elem[1] = 22;
	L.elem[2] = 33;
	L.length = 3;
	
	ElemType val;
	int n;
	cout << "要取的元素序号：" ;
	cin >> n;
	if(GetElem(L,n,val)==OK){
		cout << "第"<< n <<"号元素："<< val << endl;	
	}
	else cout << "取值失败，位序非法"<< endl;

	DestroyList(L);
	return 0;
}
