#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <climits>

using std::ostream;
using std::istream;
using std::vector;
using std::string;
using std::pair; 
using std::make_pair;
using std::default_random_engine; 
using std::uniform_int_distribution;

template<typename value_type>
value_type getRandomNumber(value_type min, value_type max) {
	static default_random_engine engine(time(NULL));
	uniform_int_distribution<value_type> distribution(min, max);
	
	return distribution(engine);
}

template<typename size_type, typename value_type>
class FixedSet {
private:
	const double multiplierCapacity;
	const double multiplierMaximumCollisionsCount;
	// first prime number > 2 * 10^9 + 1
	static const value_type KEY_MAX = 2000000011;

private:
	size_type dataSize;
	size_type capacity;
	
	pair<value_type, value_type> firstHash;
	vector<pair<value_type, value_type> > secondHashCoefficients;
	
	vector<vector<value_type> > buckets;
	vector<vector<value_type> > fixedSet;
	
private:
	void fillBuckets(const vector<value_type> & numbers);
	void fillFixedSet(const vector<value_type> & numbers);
	void setRandomPair(pair<value_type, value_type> & pair);
	size_type hash(const pair<value_type, value_type> & hashCoefficients, size_type capacity, value_type number);
	
public:
	FixedSet();
	bool contains(value_type number);
	void initialize(const vector<value_type> & numbers);

};

template<typename size_type, typename value_type>
class FixedSetSolver {
private:
	FixedSet<size_type, value_type> fixedSet;
	vector<value_type> numbers;
	
	const string ANSWER_YES;
	const string ANSWER_NO;
	
	vector<value_type> requests;
	vector<bool> answer;
	
public:
	FixedSetSolver(): ANSWER_YES("Yes"), ANSWER_NO("No") {}
	void inputData(istream& inputStream);
	void solve();
	void outputData(ostream& outputStream);
	
};

template<typename size_type, typename value_type>
void FixedSetSolver<size_type, value_type>::inputData(istream& inputStream) {
	size_type dataLength;
	inputStream >> dataLength;
	numbers.clear();
	numbers.resize(dataLength);
	
	for (size_type i = 0; i < dataLength; ++i) {
		inputStream >> numbers[i];
	}
	
	size_type requestsCount;
	inputStream >> requestsCount;
	requests.clear();
	requests.resize(requestsCount);
	
	for (size_type i = 0; i < requestsCount; ++i) {
		inputStream >> requests[i];
	}
}

template<typename size_type, typename value_type>
void FixedSetSolver<size_type, value_type>::solve() {
	fixedSet.initialize(numbers);
	
	const size_type size = requests.size();
	answer.clear();
	answer.resize(size);
	
	for (size_type i = 0; i < size; ++i) {
		bool result = fixedSet.contains(requests[i]);
		answer[i] = result;
	}
}

template<typename size_type, typename value_type>
void FixedSetSolver<size_type, value_type>::outputData(ostream& outputStream) {
	const size_type size = answer.size();
	for (size_type i = 0; i < size; ++i) {
		if (answer[i] == true) {
			outputStream << ANSWER_YES << '\n';
		}
		else {
			outputStream << ANSWER_NO << '\n';
		}
	}
}

template<typename size_type, typename value_type>
FixedSet<size_type, value_type>::FixedSet(): multiplierCapacity(5.0),
											 multiplierMaximumCollisionsCount(2 / multiplierCapacity + 4.0) {}

template<typename size_type, typename value_type>
bool FixedSet<size_type, value_type>::contains(value_type number) {
	size_type firstHashIndex = hash(firstHash, capacity, number);
	const size_type size = fixedSet[firstHashIndex].size();
	
	if (size == 0) {
		return false;
	}
	
	size_type secondHashIndex = hash(secondHashCoefficients[firstHashIndex], size, number);
	if (fixedSet[firstHashIndex][secondHashIndex] != number) {
		return false;
	}
	
	return true;
}
 
template<typename size_type, typename value_type>
void FixedSet<size_type, value_type>::initialize(const vector<value_type> & numbers) {
	dataSize = numbers.size();
	capacity = dataSize * multiplierCapacity;
	
	fillBuckets(numbers);
	fillFixedSet(numbers);
}

template<typename size_type, typename value_type>
void FixedSet<size_type, value_type>::fillBuckets(const vector<value_type> & numbers) {
	size_type collisionsCount = 0;
	const size_type maximumCollisionsCount = dataSize * multiplierMaximumCollisionsCount;
	
	while(true) {
		setRandomPair(firstHash);
		buckets.clear();
		buckets.resize(capacity);
		
		for (size_type i = 0; i < dataSize; ++i) {
			value_type nextNumber = numbers[i];
			value_type hashIndex = hash(firstHash, capacity, nextNumber);
			
			size_type sizeOfBucket = buckets[hashIndex].size();
			collisionsCount += 2 * sizeOfBucket + 1;
			if (collisionsCount > maximumCollisionsCount) {
				break;
			}
			buckets[hashIndex].push_back(nextNumber);
		}
		
		if (collisionsCount <= maximumCollisionsCount) {
			break;
		}
	}
}

template<typename size_type, typename value_type>
void FixedSet<size_type, value_type>::fillFixedSet(const vector<value_type> & numbers) {
	fixedSet.clear();
	fixedSet.resize(capacity);
	
	secondHashCoefficients.clear();
	secondHashCoefficients.resize(capacity);
	
	
	for (size_type i = 0; i < capacity; ++i) {
		const size_type size = buckets[i].size();
		if (size != 0) {
			while (true) {
				fixedSet[i].clear();
				fixedSet[i].resize(size * size, INT_MAX);
				setRandomPair(secondHashCoefficients[i]);
				bool needRebuild = false;
				
				for (size_type j = 0; j < size; ++j) {
					value_type nextNumber = buckets[i][j];
					size_type secondHashIndex = hash(secondHashCoefficients[i], size * size, nextNumber);
				
					if (fixedSet[i][secondHashIndex] != INT_MAX) {
						needRebuild = true;
						break;
					}
					
					fixedSet[i][secondHashIndex] = nextNumber;
				}
				
				if (!needRebuild) {
					break;
				}
			}
		}
	}
}

template<typename size_type, typename value_type>
void FixedSet<size_type, value_type>::setRandomPair(pair<value_type, value_type> & pair) {
	pair.first = getRandomNumber((value_type) 1, KEY_MAX);
	pair.second = getRandomNumber((value_type) 0, KEY_MAX);
}

template<typename size_type, typename value_type>
size_type FixedSet<size_type, value_type>::hash(const pair<value_type, value_type> & hashCoefficients,
												size_type capacity, value_type key) {
	// normalizedKey is between 1 and 2^10 + 1
	long long normalizedKey = key + 1000000001;
	return (((hashCoefficients.first * normalizedKey + hashCoefficients.second) % KEY_MAX) % capacity);
}

int main() {
	FixedSetSolver<unsigned int, int> solver;
	solver.inputData(std::cin);
	solver.solve();
	solver.outputData(std::cout);
	
	return 0;
}

