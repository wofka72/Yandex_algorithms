#include <iostream>
#include <vector>
#include <climits>

using std::cout;
using std::cin;
using std::vector;

void createSparseTable(const vector<int> & numbers, int N);
void fillDeg2Vector(int N);
int maxInBounds(const vector<int> & numbers, int left, int right);

vector<vector<int> > sparseTable;
vector<int> deg2geNumber;

	
int main() {
	int N;
	cin >> N;
	
	fillDeg2Vector(N);
	vector<int> numbers;
	numbers.resize(1 << deg2geNumber[N - 1]);
	int count = (1 << deg2geNumber[N - 1]);
	for (int i = 0; i < count; ++i) {
		if (i < N) {
			cin >> numbers[i];
		}
		else {
			numbers[i] = INT_MIN;
		}
	}
	createSparseTable(numbers, N);
	
	numbers.clear();
	
	int Q;
	cin >> Q;
	
	for (int i = 0; i < Q; ++i) {
		int left, right;
		cin >> left >> right;
		
		cout << maxInBounds(numbers, left, right) << '\n';
	}
	
	return 0;
}


void createSparseTable(const vector<int> & numbers, int N) {
	const int numbersSize = numbers.size();
	sparseTable.clear();
	sparseTable.resize(deg2geNumber[N - 1]);
	
	for (int row = 0; row < deg2geNumber[N - 1]; ++row) {
		int countGroups = numbersSize - (1 << (1 + row)) + 1;
		sparseTable[row].resize(countGroups);
	}
	
	for (int row = 0, group = 0; group < numbersSize - 1; ++group) {
		sparseTable[row][group] = std::max(numbers[group], numbers[group + 1]);
	}
	
	for (int row = 1; row < deg2geNumber[N - 1]; ++row) {
		const int size = (1 << (1 + row));
		
		for (int step = size / 2, group = 0; group < numbersSize - size + 1; ++group) {
			sparseTable[row][group] = std::max(sparseTable[row - 1][group], sparseTable[row - 1][group + step]);
		}
	}
}

void fillDeg2Vector(int N) {
	deg2geNumber.clear();
	deg2geNumber.resize(++N);
	
	deg2geNumber[0] = 0;
	for (int i = 1; i < N; ++i) {
		deg2geNumber[i] = 1 + deg2geNumber[i / 2];
	}
}

int maxInBounds(const vector<int> & numbers, int left, int right) {
	if (right == left) {
		return numbers[left - 1];
	}
	
	int degreeOfTwo = deg2geNumber[right - left + 1] - 2;
	
 	int maxFirstGroup = sparseTable[degreeOfTwo][left - 1];
	int maxSecondGroup = sparseTable[degreeOfTwo][right - (1 << (1 + degreeOfTwo))];
	return std::max(maxFirstGroup, maxSecondGroup);
}

