#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <climits>

using std::vector;
using std::string;
using std::istream;
using std::ostream;
using std::set;

const char SMALL_SYMBOL = 'a' - 1;

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
size_type minimalGreaterEqualDeg2(size_type size) {
    size_type deg2 = 1;
    while (deg2 < size) {
        deg2 <<= 1;
    }
    return deg2;
}

template <typename data_type, typename size_type>
class IntervalTree {
private:
    struct Interval {
        size_type left;
        size_type right;

        Interval() : left(0), right(0)
        {}

        inline bool isElement() {
            return (left == right);
        }

        inline size_type length() {
            return (right - left + 1);
        }
    };

private:
    size_type elementsCount;
    size_type arrayLength;
    vector<data_type> elements;
    vector<Interval> intervals;
    vector<data_type> minimums;

public:
    IntervalTree(const vector<data_type>& initialArray);
    void makeIntervalTree(const vector<data_type>& initialArray);
    void initializeIntervals();

public:
    inline data_type getMinimum(size_type left, size_type right) {
        return getMinimumRecursive(left, right, 1);
    }
    data_type getMinimumRecursive(size_type left, size_type right, size_type vertex);

};

template <typename data_type, typename size_type>
IntervalTree<data_type, size_type>::IntervalTree(const vector<data_type>& initialArray) {
    elementsCount = minimalGreaterEqualDeg2(initialArray.size());
    arrayLength = (elementsCount << 1) - 1;

    initializeIntervals();
    makeIntervalTree(initialArray);
}

template<typename data_type, typename size_type>
void IntervalTree<data_type, size_type>::makeIntervalTree(const vector<data_type>& initialArray) {
    elements.resize(elementsCount);
    for (size_type i = 0; i < initialArray.size(); ++i) {
        elements[i] = initialArray[i];
    }
    minimums.resize(arrayLength);

    for (size_type vertex = arrayLength; vertex > 0; --vertex) {
        if (intervals[vertex - 1].length() == 1) {
            minimums[vertex - 1] = elements[vertex - elementsCount];
        }
        else {
            minimums[vertex - 1] = std::min(minimums[2 * vertex - 1], minimums[2 * vertex]);
        }
    }
}

template<typename data_type, typename size_type>
void IntervalTree<data_type, size_type>::initializeIntervals() {
    intervals.resize(arrayLength);
    intervals[0].left = 0;
    intervals[0].right = elementsCount - 1;

    for (size_type i = 1; i < arrayLength; ++i) {
        size_type parentVertex = (i - 1) / 2;
        size_type middle = (intervals[parentVertex].left + intervals[parentVertex].right) / 2;
        bool isLeftSonOfHisParent = (i % 2 == 1);

        if (isLeftSonOfHisParent) {
            intervals[i].left = intervals[parentVertex].left;
            intervals[i].right = middle;
        }
        else {
            intervals[i].left = middle + 1;
            intervals[i].right = intervals[parentVertex].right;
        }
    }
}

template <typename data_type, typename size_type>
data_type IntervalTree<data_type, size_type>::getMinimumRecursive(
                                                    size_type left, size_type right, size_type vertex) {
    if (left > intervals[vertex - 1].right || right < intervals[vertex - 1].left) {
        return INT_MAX;
    }
    if (left <= intervals[vertex - 1].left && right >= intervals[vertex - 1].right) {
        return minimums[vertex - 1];
    }
    return std::min(getMinimumRecursive(left, right, 2 * vertex),
                    getMinimumRecursive(left, right, 2 * vertex + 1));
}


template <typename size_type>
class ISuffarayBuilder {
public:
    virtual vector<size_type> build(const string& s) = 0;
};

template <typename size_type>
class FastSuffixArrayBuilder : public ISuffarayBuilder<size_type> {
public:
    virtual vector<size_type> build(const string& initialString) {
        string tandemString = initialString;
        tandemString += SMALL_SYMBOL;

        size_type length = tandemString.size();
        vector<size_type> result(length);

        vector<size_type> class_num(length);
        vector<size_type> classes_count(std::max(length, (size_type)27));
        for (size_type i = 0; i < length; ++i) {
            class_num[i] = tandemString[i] - SMALL_SYMBOL;
            classes_count[class_num[i]] += 1;
        }
        for (size_type i = 1; i < classes_count.size(); ++i) {
            classes_count[i] += classes_count[i - 1];
        }
        for (size_type i = 0; i < length; ++i) {
            size_type position = classes_count[class_num[i]] - 1;
            classes_count[class_num[i]] -= 1;
            result[position] = i;
        }

        for (size_type level = 0; (1 << level) < length; ++level) {
            // invariant: suffixes of length (2^level) are correctly sorted
            size_type step = (1 << level);
            fill(begin(classes_count), end(classes_count), 0);
            for (size_type i = 0; i < length; ++i) {
                classes_count[class_num[i]] += 1;
            }
            for (size_type i = 1; i < classes_count.size(); ++i) {
                classes_count[i] += classes_count[i - 1];
            }

            vector<size_type> new_result(length);
            for (size_type i = length - 1; i != -1; --i) {
                size_type second_position = result[i];
                size_type first_position = (second_position - step + length) % length;

                size_type position = classes_count[class_num[first_position]] - 1;
                classes_count[class_num[first_position]] -= 1;
                new_result[position] = first_position;
            }
            result = new_result;

            vector<size_type> new_class_num(length);
            new_class_num[result[0]] = 0;
            for (size_type i = 1; i < length; ++i) {
                size_type prev_pos1 = result[i - 1], cur_pos1 = result[i];
                size_type prev_pos2 = (prev_pos1 + step) % length, cur_pos2 = (cur_pos1 + step) % length;
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

        for (size_type i = 0; i + 1 < result.size(); ++i) {
            result[i] = result[i + 1];
        }
        result.pop_back();

        return result;
    }
};

template <typename size_type>
class SameStringsSolver {
private:
    string initialString;
    size_type length;
    vector<size_type> suffixArray;
    vector<size_type> lcpArray;
    vector<size_type> positions;

    vector<size_type> sameSubstringArray;

    void findAllSameStrings();

public:
    void inputData(istream& inputStream);
    void solve();
    void outputData(ostream& outputStream);

};

template <typename size_type>
void SameStringsSolver<size_type>::inputData(istream& inputStream) {
    inputStream >> initialString;
    length = initialString.length();
}

template <typename size_type>
void SameStringsSolver<size_type>::solve() {
    FastSuffixArrayBuilder<size_type> suffixArrayBuilder;
    suffixArray = suffixArrayBuilder.build(initialString);
    positions.resize(length);
    for (size_type i = 0; i < length; ++i) {
        positions[suffixArray[i]] = i;
    }

    KasaiLCPBuilder<size_type> lcpBuilder;
    lcpArray = lcpBuilder.build(initialString, suffixArray);

    findAllSameStrings();
}

template <typename size_type>
void SameStringsSolver<size_type>::outputData(ostream& outputStream) {
    if (sameSubstringArray.size() != 0) {
        outputStream << sameSubstringArray[0];
    }
    for (size_type i = 1; i < length; ++i) {
        outputStream << ' ' << sameSubstringArray[i];
    }
}

template <typename size_type>
void SameStringsSolver<size_type>::findAllSameStrings() {
    IntervalTree<size_type, size_type> intervalTree(lcpArray);
    set<size_type> positionsSet;
    positionsSet.insert(length);

    sameSubstringArray.resize(length);
    for (size_type i = 0; i < length; ++i) {
        size_type position = positions[i];

        size_type maximalSameSubstring = 0;
        typename set<size_type>::iterator minimalGreater = positionsSet.upper_bound(position);
        if (*minimalGreater != length) {
            size_type newPretendent = intervalTree.getMinimum(position, *minimalGreater - 1);
            if (newPretendent > maximalSameSubstring) {
                maximalSameSubstring = newPretendent;
            }
        }
        typename set<size_type>::iterator maximalLess = --minimalGreater;
        if (maximalLess != positionsSet.end()) {
            size_type newPretendent = intervalTree.getMinimum(*maximalLess, position - 1);
            if (newPretendent > maximalSameSubstring) {
                maximalSameSubstring = newPretendent;
            }
        }

        positionsSet.insert(position);
        sameSubstringArray[i] = maximalSameSubstring;
    }
}


int main() {
    SameStringsSolver<unsigned long long> solver;

    std::freopen("input.txt", "r", stdin);
    std::freopen("output.txt", "w", stdout);

    solver.inputData(std::cin);
    solver.solve();
    solver.outputData(std::cout);

    return 0;
}
