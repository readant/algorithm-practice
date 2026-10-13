#include <iostream>
using namespace std;
const int MOD = 998244353;

long long qpow(long long base, long long exp) {
	long long res = 1;
	while(exp > 0){
		if(exp & 1) res = res * base % MOD;
		base = base * base % MOD;
		exp >>= 1;
	}
	return res;
}

int main()
{
	int n = 2026;
	long long pow2 = qpow(2, n-1);
	long long ans = 1LL * n * pow2 % MOD;
	cout << ans << endl;
	return 0;
}

