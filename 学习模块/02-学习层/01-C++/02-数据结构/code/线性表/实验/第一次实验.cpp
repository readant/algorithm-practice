#include <cstdlib>
#include <iostream>
using namespace std;
#define MAXSIZE 100


// 定义顺序表
typedef struct SqList{
	char *elem;
	int length;
}SqList;

// 初始化顺序表
void InitList(SqList &L){
	L.elem = new char[MAXSIZE];
	if(!L.elem) exit(-2);  // OVERFLOW 存储分配失败
	L.length = 0;
}

// 插入
int ListInsert(SqList &L,int i,char e){
	if((i < 1)||(i > L.length + 1)) return 0;  // i值不合法，不在长度中
	if(L.length == MAXSIZE) return 0; // 表满了
	for(int j = L.length - 1;j >=i-1;j--)
		L.elem[j + 1] = L.elem[j];
	L.elem[i - 1] = e;
	++L.length;
	return 1;
}

// 输出顺序表
void ListShow(SqList &L){
	for(int i = 1;i<=L.length;i++){
		 cout << L.elem[i - 1] <<" ";
	}
	cout << endl;
}

// 删除顺序表
int ListDelete(SqList &L,int i){
	if((i < 1) || (i > L.length)) return 0; // i值不合法 
	for(int j = i;j< L.length;j++)
		L.elem[j - 1] = L.elem[j];
	--L.length;
	return 1;
}

// 查找
int LocateElem(SqList L,char e){
	for(int i = 0;i < L.length;i++)
		if(L.elem[i] == e) return i + 1;
	return 0;
}

// 取值
int GetElem(SqList L, int i, char &e)
{// 取第 i 个元素值，通过 e 返回
	if(i < 1 || i > L.length) return 0;  // 越界检查
	e = L.elem[i-1];                     // 随机存取
	return 1;
}
int main(){
	SqList L;
	// 初始化
	InitList(L);
	char c;
	// 创建一个顺序表，表中元素依次为：s、t、d、i、o（均为char类型）；
	cout << "输入五个元素："<<endl;
	for(int i=1;i<=5;i++){
		cin >> c;
		ListInsert(L,i,c);
	}
	// 输出顺序表
	cout << "当前顺序表： ";
	ListShow(L);
	
	// 查找顺序表中是否包含'b'字符；
	if(LocateElem(L,'b')) cout << "顺序表中包含'b'字符"<< endl;
	else cout << "顺序表中不包含'b'字符"<<endl;
	
	// 从顺序表中取出第3个元素并返回其值；
	char e;
	if(GetElem(L,3,e)){
		cout << "第3个元素为："<< e <<endl;
	}
	else{
		cout << "取值失败，位置非法"<<endl;
	}
	
	//在第3个元素后面插入3个新的元素依次为：l、i、b；
	ListInsert(L,4,'l');
	ListInsert(L,5,'i');
	ListInsert(L,6,'b');
	// 输出顺序表
	cout << "插入三个新元素后的顺序表： ";
	ListShow(L);
	
	//将顺序表中的最后两个元素删除；
	ListDelete(L,L.length);
	//此刻7变成最后一个，直接重复操作
	ListDelete(L,L.length);
	// 输出顺序表
	cout << "删除最后两个元素后的顺序表： ";
	ListShow(L);
	
	//再次查找顺序表中是否包含'b'字符；
	if(LocateElem(L,'b')) cout << "顺序表中包含'b'字符"<< endl;
	else cout << "顺序表中不包含'b'字符"<<endl;
	//将顺序表中的元素输出。
	cout << "最终顺序表："<< endl;
	ListShow(L);
	// 结束，释放内存
	delete [] L.elem;
	
	return 0;
}
