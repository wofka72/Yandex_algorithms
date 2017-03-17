#define _CRT_SECURE_NO_WARNINGS

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
class ISuffarayBuilder {
public:
    virtual vector<size_type> build(const string& s) = 0;
};

template <typename size_type>
class NaiveBuilder : public ISuffarayBuilder<size_type> {
public:
    virtual vector<size_type> build(const string& s) {
        vector<pair<string, size_type>> buffer;
        for (size_t i = 0; i < s.size(); ++i) {
            buffer.emplace_back(s.substr(i), i);
        }
        sort(begin(buffer), end(buffer));

        vector<size_t> expected;
        for (auto&& x : buffer) {
            expected.push_back(x.second);
        }
        return expected;
    }
};

class SuperBuilder : public ISuffarayBuilder {
public:
    virtual vector<size_t> build(const string& ss)  {
        string s = ss;
        s += char('a' - 1);
        int n = s.size();
        vector<size_t> result(n);

        vector<size_t> class_num(n);
        vector<size_t> classes_count(max(n, 27));
        for (size_t i = 0; i < n; ++i) {
            class_num[i] = s[i] - ('a' - 1);
            classes_count[class_num[i]] += 1;
        }
        for (size_t i = 1; i < classes_count.size(); ++i) {
            classes_count[i] += classes_count[i - 1];
        }
        for (size_t i = 0; i < n; ++i) {
            size_t position = classes_count[class_num[i]] - 1;
            classes_count[class_num[i]] -= 1;
            result[position] = i;
        }

        for (int level = 0; (1 << level) < n; ++level) {
            // invariant: suffixes of length (2^level) are correctly sorted
            int step = 1 << level;
            fill(begin(classes_count), end(classes_count), 0);
            for (int i = 0; i < n; ++i) {
                classes_count[class_num[i]] += 1;
            }
            for (size_t i = 1; i < classes_count.size(); ++i) {
                classes_count[i] += classes_count[i - 1];
            }

            vector<size_t> new_result(n);
            for (int i = n - 1; i >= 0; --i) {
                int second_position = result[i];
                int first_position = (second_position - step + n) % n;

                int position = classes_count[class_num[first_position]] - 1;
                classes_count[class_num[first_position]] -= 1;
                new_result[position] = first_position;
            }
            result = new_result;

            vector<size_t> new_class_num(n);
            new_class_num[result[0]] = 0;
            for (int i = 1; i < n; ++i) {
                int prev_pos1 = result[i - 1], cur_pos1 = result[i];
                int prev_pos2 = (prev_pos1 + step) % n, cur_pos2 = (cur_pos1 + step) % n;
                if (class_num[prev_pos1] != class_num[cur_pos1]
                    || class_num[prev_pos2] != class_num[cur_pos2]) {
                    new_class_num[cur_pos1] = new_class_num[prev_pos1] + 1;
                }
                else {
                    new_class_num[cur_pos1] = new_class_num[prev_pos1];
                }
            }
            class_num = new_class_num;
        }

        for (size_t i = 0; i + 1 < result.size(); ++i) {
            result[i] = result[i + 1];
        }
        result.pop_back();

        return result;
    }
};

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

template <typename size_type>
bool check(ILCPBuilder<size_type>* builder, const string& initialString, const vector<size_type>& suffixArray) {
    vector<size_type> got = builder->build(initialString, suffixArray);
    if (got.size() != initialString.size()) {
        return false;
    }

    for (size_type i = 0; i + 1 < initialString.size(); ++i) {
        string str1 = initialString.substr(suffixArray[i], got[i]);
        string str2 = initialString.substr(suffixArray[i + 1], got[i]);
        if (str1 != str2) {
            return false;
        }
    }
    return true;
}

template <typename size_type>
bool stress_testing(ILCPBuilder<size_type>* lcpBuilder, ISuffarayBuilder* suffarrayBuilder, size_type tests = 10000) {
    for (size_type i = 0; i < tests; ++i) {
        int length = (rand() % 100) + 4;
        string test;
        for (int j = 0; j < length; ++j) {
            test += char((rand() % 26) + 'a');
        }
        vector<size_type> suffarray = suffarrayBuilder->build(test);
        if (check(lcpBuilder, test, suffarray)) {
            std::cerr << "1" << std::endl;
        }
        else {
            std::cerr << "Test failed '" << test << "'" << std::endl;
            system("pause");
            return false;
        }
    }
    std::cerr << tests << " tests passed" << std::endl;
    return true;
}


int main() {
    LCPSolver<unsigned> solver;
    std::ios_base::sync_with_stdio(false);

    std::freopen("input.txt", "r", stdin);
    std::freopen("output.txt", "w", stdout);

    KasaiLCPBuilder<size_t> kasaiLCPBuilder;
    SuperBuilder fastSuffarrayBuilder;

    string abacaba = "abacaba";
    vector<size_t> sufArray = fastSuffarrayBuilder.build(abacaba);
    bool abacabaCheck = check(&kasaiLCPBuilder, abacaba, sufArray);


    srand(time(NULL));
    if (stress_testing(&kasaiLCPBuilder, &fastSuffarrayBuilder)) {
        std::cerr << "ALL IS OK!\n";
    }
    else {
        std::cerr << "SMTH GOES WRONG!\n";
    }

    solver.inputData(std::cin);
    solver.solve();
    solver.outputData(std::cout);

    return 0;
}
