#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> nums;
    int x;
    while (cin >> x && x != 0) {
        nums.push_back(x);
    }
    
    for (auto it = nums.rbegin(); it != nums.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    return 0;
}