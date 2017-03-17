#include <iostream>

using std::cin;
using std::cout;

long long quick_deg(long long a, long long b, long long c) {
	if (b == 0) {
		return 1 % c;
	}
	
	long long result = quick_deg(a, b / 2, c);
	if (b % 2 == 1) {
		return (((result * result) % c) * (a % c)) % c;
	}
	return (result * result) % c;
}

main() {
	long long Q;
	cin >> Q;
	
	for (long long i = 0; i < Q; ++i) {
		long long a, b, c;
		cin >> a >> b >> c;
		cout << quick_deg(a, b, c) << '\n';
	}
	
	return 0;
}

