#include <iostream>
#include <stdlib.h>

#define MAXSIZE 100
using namespace std;

typedef struct{
	int *elem;
	int length;
}SqList;

bool InitSqList(SqList &L){
	L.elem = new int[MAXSIZE];
	
	L.length = 0;
	return true;
}


int main(){
	SqList L;
	
	return 0;
}
