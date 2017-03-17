#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

using std::string;
using std::vector;
using std::istream;
using std::ostream;
using std::pair;
using std::max;


template <typename size_type>
class ILCPBuilder {
public:
    virtual vector<size_type> build(const string& initialString, const vector<size_type>& suffixArray) = 0;
};

template <typename size_type>
class KasaiLCPBuilder : public ILCPBuilder<size_type> {
public:
    virtual vector<size_type> build(const string& initialString, const vector<size_type>& suffixArray) {
        size_type length = initialString.length();
        vector<size_type> lcpArray(length);
        size_type currentLCP = 0;
        vector<size_type> positions(length);
        for (size_type i = 0; i < length; ++i) {
            positions[suffixArray[i]] = i;
        }

        for (size_type i = 0; i < length; ++i) {
            if (currentLCP > 0) {
                --currentLCP;
            }
            if (positions[i] == length - 1) {
                lcpArray[length - 1] = -1;
            }
            else {
                size_type currentPosition = suffixArray[positions[i] + 1];
                while ((std::max(i + currentLCP, currentPosition + currentLCP) < length) &&
                    (initialString[i + currentLCP] == initialString[currentPosition + currentLCP])) {
                    ++currentLCP;
                }
                lcpArray[positions[i]] = currentLCP;
            }
        }

        return lcpArray;
    }
};

template <typename size_type>
class LCPSolver {
private:
    string initialString;
    size_type length;
    vector<size_type> suffixArray;

private:
    vector<size_type> lcpArray;
    vector<bool> arrayForCount;

    size_type countDifferentSubstrings;
    size_type maxLCP;
    size_type countDifferentLCP;

public:
    void inputData(istream& inputStream);
    void solve();
    void outputData(ostream& outputStream);

};

template <typename size_type>
void LCPSolver<size_type>::inputData(istream& inputStream) {
    inputStream >> initialString;
    length = initialString.length();
    suffixArray.resize(length);

    for (size_type i = 0; i < length; ++i) {
        inputStream >> suffixArray[i];
    }
}

template <typename size_type>
void LCPSolver<size_type>::solve() {
    KasaiLCPBuilder<size_type> lcpBuilder;
    vector<size_type> lcpArray = lcpBuilder.build(initialString, suffixArray);

    // computing count of the different substrings of initial strings
    size_type sumLCP = 0;
    for (size_type i = 0; i < length - 1; ++i) {
        sumLCP += lcpArray[i];
    }
    countDifferentSubstrings = (length * (length + 1)) / 2 + 1;
    countDifferentSubstrings -= sumLCP;

    // computing the maximum in the LCP-array
    maxLCP = 0;
    for (size_type i = 0; i < length - 1; ++i) {
        maxLCP = std::max(maxLCP, lcpArray[i]);
    }

    // computing count of different values int the LCP-array
    arrayForCount.resize(length - 1);
    for (size_type i = 0; i < length - 1; ++i) {
        arrayForCount[lcpArray[i]] = true;
    }

    countDifferentLCP = 0;
    for (size_type i = 0; i < length - 1; ++i) {
        if (arrayForCount[i] == true) {
            ++countDifferentLCP;
        }
    }
}

template <typename size_type>
void LCPSolver<size_type>::outputData(ostream& outputStream) {
    outputStream << countDifferentSubstrings
        << ' ' << maxLCP
        << ' ' << countDifferentLCP;
}


int main() {
    LCPSolver<unsigned long long> solver;
    std::ios_base::sync_with_stdio(false);

    std::freopen("input.txt", "r", stdin);
    std::freopen("output.txt", "w", stdout);

    solver.inputData(std::cin);
    solver.solve();
    solver.outputData(std::cout);

    return 0;
}
