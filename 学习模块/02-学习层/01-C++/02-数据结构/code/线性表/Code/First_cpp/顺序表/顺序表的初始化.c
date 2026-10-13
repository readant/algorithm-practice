#include <stdio.h>
#include <stdlib.h>

// 预定义常量（教材标准）
#define OK 1
#define ERROR 0
#define OVERFLOW -2
#define MAXSIZE 100

// 类型定义
typedef int ElemType;
typedef int Status;

// 动态分配顺序表结构体
typedef struct
{
	ElemType *elem;
	int length;
} SqList;

// 【算法2.1】初始化顺序表（C语言版，用指针）
Status InitList(SqList *L)
{
	L->elem = (ElemType *)malloc(MAXSIZE * sizeof(ElemType));
	if (!L->elem)
		return OVERFLOW;
	L->length = 0;
	return OK;
}

// 销毁顺序表，释放内存（上机练习用，考试可不写）
void DestroyList(SqList *L)
{
	free(L->elem);
	L->elem = NULL;
	L->length = 0;
}

// main测试，仅上机使用，考试不要写main
int main(void)
{
	SqList L;
	Status res = InitList(&L);  // 传入结构体地址
	
	if (res == OK)
	{
		printf("顺序表初始化成功\n");
		printf("当前表长 = %d\n", L.length);
	}
	else
	{
		printf("初始化失败，内存溢出\n");
	}
	
	DestroyList(&L);
	return 0;
}

