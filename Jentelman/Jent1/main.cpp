#include <iostream>
#include <vector>
#include <string>

using std::cin;
using std::cout;
using std::string;
using std::vector;

class BigDecimal {
private:
	vector<int> number;
	int length;
	bool isNegative;
	
public:
	BigDecimal(): length(1), isNegative(false) {
		number.push_back(0);
	}
	BigDecimal(int length_): length(length_), isNegative(false) {
		number.resize(length, 0);
	}
	BigDecimal(string stringNumber) {
		if (stringNumber.length() > 0 && stringNumber[0] == '-') {
			isNegative = true;
			length = stringNumber.length() - 1;
		}
		else {
			isNegative = false;
			length = stringNumber.length();
			number.resize(length);
		}
		number.resize(length);
		
		if (isNegative) {
			for (int i = length; i > 0; --i) {
				int digit = stringNumber[i] - '0';
				number[length - i] = digit;
			}
		}
		else {
			for (int i = 0; i < length; ++i) {
				int digit = stringNumber[length - i - 1] - '0';
				number[i] = digit;
			}
		}
	}
	
	BigDecimal& operator* (const BigDecimal& otherNumber) {
		int newLength = length + otherNumber.length;
		
		BigDecimal* newNumber = new BigDecimal(newLength);
		for (int i = 0; i < length; ++i) {
			for (int j = 0; j < otherNumber.length; ++j) {
				newNumber->number[i + j] += number[i] * otherNumber.number[j];
			}
		}
		
		newNumber->normalize();
		
		if ((isNegative && !otherNumber.isNegative) || (!isNegative && otherNumber.isNegative)) {
			newNumber->isNegative = true;
		}
		
		return *newNumber;
	}
	
	void normalize() {
		number.push_back(0);
		
		for (int i = 0; i < length; ++i) {
			if (number[i] >= 10) {
				int nextDischarge = number[i] / 10;
				number[i] %= 10;
				
				number[i + 1] += nextDischarge;
			}
		}
		
		int lastDischarge = number[length];
		int position = length;
		while (lastDischarge > 0) {
			number.push_back(lastDischarge / 10);
			number[position++] %= 10;
		}
	}
	
	friend std::ostream& operator<< (std::ostream& outputStream, const BigDecimal& bigNumber) {
		int firstPosition = bigNumber.length - 1;
		
		if (bigNumber.isNegative) {
			outputStream << '-';
		}
		
		while (firstPosition >= 0 && bigNumber.number[firstPosition] == 0) {
			--firstPosition;
		}
		
		if (firstPosition == -1) {
			outputStream << 0;
		}
		else {
			while(firstPosition >= 0) {
				outputStream << bigNumber.number[firstPosition--];
			}
		}
		
		outputStream << '\n';
	}
};

int main() {
	string stringNumber1;
	string stringNumber2;
	cin >> stringNumber1 >> stringNumber2;
	BigDecimal bigNumber(stringNumber1);
	BigDecimal bigNumber_2(stringNumber2);
	
	//cout << "bigNumber1 = " << bigNumber;
	//cout << "bigNumber2 = " << bigNumber_2;
	cout << bigNumber * bigNumber_2;
}

