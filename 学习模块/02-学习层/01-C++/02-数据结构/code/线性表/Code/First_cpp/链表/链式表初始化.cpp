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

Status InitList(LinkList &L)
{
	L = new LNode;
	L->next = NuLL;
	return OK;
}

int main(){
	
	
	return 0;
}
