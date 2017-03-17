#include <iostream>
#include <vector>
#include <climits>

using std::cin;
using std::cout;
using std::vector;

struct CentralNumber {
	int number;
	  int countSameLeft;
	long long countGreaterLeft;
	long long countLowerRight;
	
	CentralNumber(int number_): number(number_), countSameLeft(0), countGreaterLeft(0), countLowerRight(0) {}
	CentralNumber(): number(1), countSameLeft(0), countGreaterLeft(0), countLowerRight(0) {}
	CentralNumber(const CentralNumber& n): number(n.number), countSameLeft(0),
										   countGreaterLeft(n.countGreaterLeft), countLowerRight(n.countLowerRight) {}
	CentralNumber operator= (const CentralNumber& n) {
		number = n.number;
		countGreaterLeft = n.countGreaterLeft;
		countLowerRight = n.countLowerRight;
		countSameLeft = n.countSameLeft;
	}
	bool operator< (const CentralNumber& n) const {
		return number < n.number;
	}
	bool operator> (const CentralNumber& n) const {
		return number > n.number;
	}
	bool operator== (const CentralNumber& n) const {
		return number == n.number;
	}
};

long long computeCountSuperInversions(vector<CentralNumber> & numbers);
void insidiousMergeSort(vector<CentralNumber> & numbers);
void merge(vector<CentralNumber> & numbers, vector<CentralNumber> & leftNumbers,
		   vector<CentralNumber> & rightNumbers);

int main() {
	int N;
	cin >> N;
	
	vector<CentralNumber> numbers(N);
	for (int i = 0; i < N; ++i) {
		cin >> numbers[i].number;
	}
	
	long long result = computeCountSuperInversions(numbers);
	cout << result << std::endl;
	
	return 0;
}


long long computeCountSuperInversions(vector<CentralNumber> & numbers) {
	const int size = numbers.size();
	insidiousMergeSort(numbers);
	
	long long sum = 0;
	for (int i = 0; i < size; ++i) {
		sum += numbers[i].countGreaterLeft * numbers[i].countLowerRight;
	}
	
	return sum;
}

void insidiousMergeSort(vector<CentralNumber> & numbers) {
	int size = numbers.size();
	if (size > 1) {
		int middle = size / 2;
		
		vector<CentralNumber> leftNumbers(&(numbers[0]), &(numbers[middle]));
		vector<CentralNumber> rightNumbers(numbers.begin() + middle, numbers.end());
		
		insidiousMergeSort(leftNumbers);
		insidiousMergeSort(rightNumbers);
		
		merge(numbers, leftNumbers, rightNumbers);
	}
}

void merge(vector<CentralNumber> & numbers, vector<CentralNumber> & leftNumbers,
		   vector<CentralNumber> & rightNumbers) {
		   	
	const int countLeft = leftNumbers.size();
	const int countRight = rightNumbers.size();
	const int count = countLeft + countRight;

	leftNumbers.push_back(*(new CentralNumber(INT_MAX)));
	rightNumbers.push_back(*(new CentralNumber(INT_MAX)));

	for (int i = 1; i < countRight; ++i) {
		if (rightNumbers[i] == rightNumbers[i - 1]) {
			rightNumbers[i].countSameLeft = 1 + rightNumbers[i - 1].countSameLeft;
		}
	}
	
	int first = 0;
	int second = 0;
	for (int i = 0; i < count; ++i) {
		if (leftNumbers[first] > rightNumbers[second]) {
			rightNumbers[second].countGreaterLeft += (countLeft - first);
			numbers[i] = rightNumbers[second];
			++second;
		}
		else if (leftNumbers[first] < rightNumbers[second]) {
			leftNumbers[first].countLowerRight += second;
			numbers[i] = leftNumbers[first];
			++first;
		}
		else {
			leftNumbers[first].countLowerRight += second - rightNumbers[second].countSameLeft;
			numbers[i] = leftNumbers[first];
			++first;
		}
	}
}

