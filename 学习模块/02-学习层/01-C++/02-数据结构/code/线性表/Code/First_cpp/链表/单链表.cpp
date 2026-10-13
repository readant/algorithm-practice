#include <iostream>
#define OK 1
#define ERROR 0
#define OVERFLOW -2
using namespace std;
typedef int Status;


typedef struct LNode {
	int data;                // 节点值
	struct LiskList * next;  // 指向下一个节点的指针
}LNode,*LinkList;

// 初始化
int InitList(LinkList &L)
{   // 构造一个空链表L
	L = new LNode;      // 生成新节点作为头节点
	L->next = NULL;     // 头结点的指针域置空
	return OK;
}
// 按位取值
int GetElem(LinkList L,int i,int &e)
{   // 带头节点的单链表 L 中根据序号 i 获取
	p = L->next;
	int j = 1;
	while(p && j < i) {
		p = p->next;
		++j;
	}
	if(!p || j > i) return ERROR;
	e = p->data;
	return OK;
}

int main() {
	LNode L;
	
	return 0;
}
