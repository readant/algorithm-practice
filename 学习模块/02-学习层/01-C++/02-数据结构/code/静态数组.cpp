#include <iostream>
using namespace std;

class Array {
private:
    int data[100];
    int size;
    
public:
    Array() : size(0) {}
    
    void pushBack(int val) {
        data[size++] = val;
    }
    
    void insert(int index, int val) {
        for (int i = size; i > index; i--) {
            data[i] = data[i - 1];
        }
        data[index] = val;
        size++;
    }
    
    void remove(int index) {
        for (int i = index; i < size - 1; i++) {
            data[i] = data[i + 1];
        }
        size--;
    }
    
    int get(int index) {
        return data[index];
    }
    
    void print() {
        for (int i = 0; i < size; i++) {
            cout << data[i] << " ";
        }
        cout << endl;
    }
};

int main() {
    Array arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);
    arr.print();
    
    arr.insert(1, 10);
    arr.print();
    
    arr.remove(2);
    arr.print();
    
    return 0;
}