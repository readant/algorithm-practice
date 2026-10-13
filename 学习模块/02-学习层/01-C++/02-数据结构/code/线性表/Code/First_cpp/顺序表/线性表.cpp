#include <iostream>
#include <cstdlib>
#include <cstdio>
using namespace std;

#define OK 1
#define TRUE 1
#define FALSE 0
#define ERROR 0
#define INFEASIBLE (-1)
#define OVERFLOW (-2)
#define LIST_INIT_SIZE 100 //线性表存储空间的初始分配量
#define LISTINCREMENT 50
#define NOTFIND (-1)

typedef int ElemType;
typedef int Status;

typedef struct {
	ElemType *elem; //储存空间基址
	int length; //当前长度
	int listsize; //当前分配的储存容量(以sizeof(Elemtype))为单位
} SqList;

Status InitList(SqList &L) {
	//构造一个空的线性表L
	L.elem = (ElemType *) malloc(LIST_INIT_SIZE * sizeof(ElemType));
	//或者用C++实现: L.elem = new ElemType[List_init_Size]
	if (!L.elem) {
		//分配失败
		return OVERFLOW;
	}
	L.length = 0; //空表长度为0
	L.listsize = LIST_INIT_SIZE; //初始存储容量
	return OK;
}

//获取指定位置的数据
Status GetElem(SqList L, int i, ElemType &e) {
	if (i < 1 || i > L.length) {
		return INFEASIBLE;
	}
	e = L.elem[i - 1];
	return OK;
}

//更新指定位置的数据
Status SetElem(SqList &L, int i, ElemType e) {
	if (i < 1 || i > L.length) {
		return INFEASIBLE;
	}
	L.elem[i - 1] = e;
	return OK;
}


// 自动扩容版尾插
Status AddElem(SqList &L, ElemType e) {
	if (L.length >= L.listsize) {
		ElemType *newbase = (ElemType *) realloc(L.elem,
		                    (L.listsize + LISTINCREMENT) * sizeof(ElemType));
		if (!newbase) return OVERFLOW;
		L.elem = newbase;
		L.listsize += LISTINCREMENT;
	}
	L.elem[L.length++] = e;
	return OK;
}

//在顺序表中指定位置插入数据
Status InsertElem(SqList &L, int position, ElemType e) {
	//position指要插入数据的位置
	//检测插入点位置是否正确(可以插在表尾)
	if (position < 1 || position > L.length + 1) {
		return INFEASIBLE;
	}
	//判断顺序表是否已满,满了就扩容
	if (L.length >= L.listsize) {
		ElemType *newbase = (ElemType *) realloc(L.elem,
		                    (L.listsize + LISTINCREMENT) * sizeof(ElemType));
		if (!newbase) return OVERFLOW;
		L.elem = newbase;
		L.listsize += LISTINCREMENT;
	}
	//要先把position位置后的所有数据都往后移动一位，而且是从后往前开始移动，一直挪到 position - 1后面那个下标的位置
	for (int i = L.length - 1; i >= position - 1; i--) {
		//交换值
		L.elem[i + 1] = L.elem[i];
	}
	//赋值
	L.elem[position - 1] = e;
	//插入数据后顺序表的长度要 +1
	L.length++;
	return OK;
}

//删除指定位置的数据
Status DeleteElem(SqList &L, int position, ElemType &e) {
	if (position < 1 || position > L.length) {
		return INFEASIBLE;
	}
	e = L.elem[position - 1];
	for (int i = position; i < L.length; i++) {
		L.elem[i - 1] = L.elem[i];
	}
	L.length--;
	return OK;
}

//查找数据位置
int FindElem(SqList &L, ElemType e) {
	if (L.length == 0) {
		return ERROR;
	} else {
		for (int i = 0; i < L.length; i++) {
			if (L.elem[i] == e) {
				return i + 1;
				break;
			}
		}
		return NOTFIND;
	}
}

//返回顺序表长度
int LengthList(SqList &L) {
	return L.length;
}

//清空顺序表
Status ClearList(SqList &L) {
	L.length = 0;
	return OK;
}

//打印顺序表数据
void printList(SqList &L) {
	for (int i = 0; i < L.length; i++) {
		cout << L.elem[i] << " ";
	}
	cout << endl;
}

//释放内存
void DestroyList(SqList &L) {
	free(L.elem);
	L.elem = nullptr;
	L.length = L.listsize = 0;
}

void menu(int option, SqList &L) {
	if (option == '1') {
		//查看顺序表
		cout << "===== 查看顺序表 ===== " << endl;
		printList(L);
		cout << endl;
	} else if (option == '2') {
		//获取指定位置的数据
		cout << "===== 获取指定位置的数据 ===== " << endl;
		ElemType e;
		int position;
		cout << "请输入要获取的数据位置：" << endl;
		cin >> position;
		int res = GetElem(L, position, e);
		if (res == INFEASIBLE) {
			cout << "获取失败，位置不合法！" << endl;
		} else {
			cout << "获取到第" << position << "个数据： " << e << endl;
		}
	} else if (option == '3') {
		//添加新数据
		cout << "===== 添加新数据 ===== " << endl;
		int new_data;
		cout << "请输入要添加的数据：";
		cin >> new_data;
		int res = AddElem(L, new_data);
		if (res) {
			cout << "添加新数据" << new_data << "成功" << endl;
		} else {
			cout << "添加失败！" << endl;
		}
	} else if (option == '4') {
		//更新指定位置的数据
		cout << "===== 更新指定位置的数据 ===== " << endl;
		ElemType e;
		int position;
		cout << "请输入要更新的数据位置：" << endl;
		cin >> position;
		cout << "请输入新的数据：" << endl;
		cin >> e;
		int res = SetElem(L, position, e);
		if (res == INFEASIBLE) {
			cout << "更新失败，位置不合法！" << endl;
		} else {
			L.elem[position - 1] = e;
			cout << "成功将第" << position << "个数据更新为： " << e << endl;
		}
	} else if (option == '5') {
		//在顺序表中指定位置插入数据
		cout << "===== 在顺序表中指定位置插入数据 ===== " << endl;
		int position;
		ElemType e;
		cout << "请输入要插入的数据位置：" ;
		cin >> position;
		cout << "请输入要插入的新数据：" ;
		cin >> e;
		int res = InsertElem(L, position, e);
		if (res == INFEASIBLE) {
			cout << "插入位置非法！" << endl;
		} else if (res == OK) {
			L.elem[position - 1] = e;
			cout << "成功在第" << position << "个位置插入数据： " << e << endl;
		} else {
			cout << "插入失败！" << endl;
		}
	} else if (option == '6') {
		//删除指定位置的数据
		cout << "===== 删除指定位置的数据 ===== " << endl;
		int position;
		ElemType e;
		cout << "请输入要删除的数据位置：" ;
		cin >> position;
		int res = DeleteElem(L, position, e);
		if (res == INFEASIBLE) {
			cout << "删除位置非法！" << endl;
		} else if (res == OK) {
			cout << "成功将第" << position << "个位置的数据： " << e << "删除" << endl;
		}
	} else {
		//查找数据位置
		cout << "===== 查找数据位置 ===== " << endl;
		ElemType e;
		cout << "请输入要查找的数据：" ;
		cin >> e;
		int res = FindElem(L, e);
		if (res == ERROR) {
			cout << "顺序表为空表！" << endl;
		} else if (res > 0) {
			cout << "成功查到" << e << "在顺序表的第： " << res << "个位置" << endl;
		} else {
			cout << "在顺序表中未查到" << e << "的位置 " << endl;
			cout << endl;
		}
	}
}


int main() {
	SqList L;
	InitList(L);
	for (int i = 1; i <= 5; i++) {
		AddElem(L, i * 10);
	} // 10 20 30 40 50
	while (1) {
		cout << "===== 功能菜单 =====" << endl;
		cout << "(1)查看顺序表" << endl;
		cout << "(2)获取指定位置的数据" << endl;
		cout << "(3)添加新数据" << endl;
		cout << "(4)更新指定位置的数据" << endl;
		cout << "(5)在顺序表中指定位置插入数据" << endl;
		cout << "(6)删除指定位置的数据" << endl;
		cout << "(7)查找数据位置" << endl;
		cout << "(8)退出" << endl;
		char n;
		cout << "请输入要选择的功能：";
		cin >> n;
		if ('1' <= n && n <= '7') {
			menu(n, L);
		} else if (n == '8') {
			cout << "退出成功！" << endl;
			break;
		} else {
			cout << "输入错误！请输入合法范围内的数！" << endl;
		}
	}
	return 0;
}


