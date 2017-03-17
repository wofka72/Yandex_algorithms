#include <iostream>

using std::cin;
using std::cout;

int main() {
	long long N;
	cin >> N;
	
	const long long divider = 1000000007;
	long long sum = 0;
	long long deg5 = 5;
	
	while (deg5 <= N) {
		long long first_last = N - deg5 + 2 + (N % deg5);
		long long count = N - (N % deg5);
		long long rem = 2 * deg5;
		
		if (first_last % 2 == 0) {
			first_last /= 2;
			rem /= 2;
		}
		
		if (count % 2 == 0 && rem % 2 == 0) {
			count /= 2;
			rem /= 2;
		}
		
		while (first_last % 5 == 0 && rem > 1) {
			first_last /= 5;
			rem /= 5;
		}
		
		while (count % 5 == 0 && rem > 1) {
			count /= 5;
			rem /= 5;
		}
		
		long long addend = ((count % divider) * (first_last % divider)) % divider;
		sum += addend;
		sum %= divider;
		
		deg5 *= 5;
	}
	
	cout << sum << std::endl;
	
	return 0;
}

