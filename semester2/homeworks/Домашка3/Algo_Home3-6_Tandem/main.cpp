#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>

using std::istream;
using std::ostream;
using std::vector;


template <typename size_type>
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

template <typename size_type>
size_type minimalLessEqualDeg2(size_type size) {
    size_type deg2 = 1;
    while (deg2 < size) {
        deg2 <<= 1;
    }
    return deg2;
}


template <typename data_type, typename size_type>
class IntervalTreeSolver {
private:
    size_type elementsCount;
    size_type arrayLength;
    vector<data_type> elements;

    vector<data_type> minimums;
    vector<data_type> maximums;
    vector<data_type> sums;

    vector<Interval<size_type>> intervals;
    vector<data_type> changeValues;

private:
    void makeIntervalTree();
    void makeIntervalTreeRecursive(size_type left, size_type right, size_type index);
    void initializeAllArrays();
    void initializeIntervals();

    inline data_type getMinimum(size_type left, size_type right) {
        return getMinimumRecursive(left, right, 1);
    }
    inline data_type getMaximum(size_type left, size_type right) {
        return getMaximumRecursive(left, right, 1);
    }
    inline data_type getSum(size_type left, size_type right) {
        return getSumRecursive(left, right, 1);
    }
    inline void addValue(size_type left, size_type right, data_type value) {
        return addValueRecursive(left, right, value, 1);
    }
    inline void updateElement(size_type index, data_type value) {
        return updateElementRecursive(index, value, 1);
    }

    data_type getMinimumRecursive(size_type left, size_type right, size_type vertex);
    data_type getMaximumRecursive(size_type left, size_type right, size_type vertex);
    data_type getSumRecursive(size_type left, size_type right, size_type vertex);
    void addValueRecursive(size_type left, size_type right, data_type value, size_type vertex);
    void updateElementRecursive(size_type index, data_type value, size_type vertex);

    void pushChangeValues(size_type vertex);
    void updateFunctions(size_type vertex);

public:
    void solve(istream& inputStream, ostream& outputStream);

};


template <typename data_type, typename size_type>
void IntervalTreeSolver<data_type, size_type>::solve(istream& inputStream, ostream& outputStream) {
    inputStream >> elementsCount;
    elementsCount = minimalLessEqualDeg2<size_type>(elementsCount);

    initializeAllArrays();

    size_type index, left, right;
    data_type value, requestType;
    inputStream >> requestType;
    size_type requestIndex = 1;
    while (requestType != 0) {
        switch (requestType) {
            case 1:
                inputStream >> index >> value;
                updateElement(index, value);
                break;

            case 2:
                inputStream >> left >> right >> value;
                addValue(left, right, value);
                break;

            case 3:
                inputStream >> left >> right;
                outputStream << getSum(left, right) << '\n';
                break;

            case 4:
                inputStream >> left >> right;
                outputStream << getMinimum(left, right) << '\n';
                break;

            case 5:
                inputStream >> left >> right;
                outputStream << getMaximum(left, right) << '\n';
                break;

            default:
                std::cerr << "This is impossible! You can't get it!";
        }
        inputStream >> requestType;
        ++requestIndex;
    }
}


template<typename data_type, typename size_type>
void IntervalTreeSolver<data_type, size_type>::initializeAllArrays() {
    arrayLength = (elementsCount << 1) - 1;

    elements.resize(elementsCount, 0);
    changeValues.resize(arrayLength);
    sums.resize(arrayLength, 0);
    maximums.resize(arrayLength, 0);
    minimums.resize(arrayLength, 0);

    intervals.resize(arrayLength);
    initializeIntervals();
}

template<typename data_type, typename size_type>
void IntervalTreeSolver<data_type, size_type>::initializeIntervals() {
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

template<typename data_type, typename size_type>
void IntervalTreeSolver<data_type, size_type>::updateElementRecursive(
    size_type index, data_type value, size_type vertex) {
    pushChangeValues(vertex);

    if (intervals[vertex - 1].isElement()) {
        elements[intervals[vertex - 1].left] = sums[vertex - 1] =
            minimums[vertex - 1] = maximums[vertex - 1] = value;
    }
    else {
        if (index <= intervals[2 * vertex - 1].right) {
            updateElementRecursive(index, value, 2 * vertex);
        }
        else {
            updateElementRecursive(index, value, 2 * vertex + 1);
        }
        updateFunctions(vertex);
    }
}


template <typename data_type, typename size_type>
void IntervalTreeSolver<data_type, size_type>::addValueRecursive(
    size_type left, size_type right, data_type value, size_type vertex) {
    pushChangeValues(vertex);

    if (left > intervals[vertex - 1].right || right < intervals[vertex - 1].left) {
        return;
    }
    else if (intervals[vertex - 1].isElement()) {
        elements[intervals[vertex - 1].left] += value;
        sums[vertex - 1] += value;
        minimums[vertex - 1] += value;
        maximums[vertex - 1] += value;
    }
    else {
        if (left <= intervals[vertex - 1].left && right >= intervals[vertex - 1].right) {
            changeValues[vertex - 1] += value;
        }
        else {
            addValueRecursive(left, right, value, 2 * vertex);
            addValueRecursive(left, right, value, 2 * vertex + 1);
            updateFunctions(vertex);
        }
    }
}

template <typename data_type, typename size_type>
data_type IntervalTreeSolver<data_type, size_type>::getSumRecursive(
    size_type left, size_type right, size_type vertex) {
    pushChangeValues(vertex);

    if (left > intervals[vertex - 1].right || right < intervals[vertex - 1].left) {
        return 0;
    }
    if (left <= intervals[vertex - 1].left && right >= intervals[vertex - 1].right) {
        return sums[vertex - 1];
    }

    size_type length = intervals[2 * vertex - 1].length();

    data_type leftSum = getSumRecursive(left, right, 2 * vertex);
    data_type leftChangeValue = changeValues[2 * vertex - 1] * length;
    data_type rightSum = getSumRecursive(left, right, 2 * vertex + 1);
    data_type rightChangeValue = changeValues[2 * vertex] * length;

    return leftSum + rightSum + leftChangeValue + rightChangeValue;
}

template <typename data_type, typename size_type>
data_type IntervalTreeSolver<data_type, size_type>::getMinimumRecursive(
    size_type left, size_type right, size_type vertex) {
    pushChangeValues(vertex);

    if (left > intervals[vertex - 1].right || right < intervals[vertex - 1].left) {
        return INT_MAX;
    }
    if (left <= intervals[vertex - 1].left && right >= intervals[vertex - 1].right) {
        return minimums[vertex - 1];
    }

    data_type left_minimum = getMinimumRecursive(left, right, 2 * vertex) + changeValues[2 * vertex - 1];
    data_type right_minimum = getMinimumRecursive(left, right, 2 * vertex + 1) + changeValues[2 * vertex];
    return std::fmin(left_minimum, right_minimum);
}

template <typename data_type, typename size_type>
data_type IntervalTreeSolver<data_type, size_type>::getMaximumRecursive(
    size_type left, size_type right, size_type vertex) {
    pushChangeValues(vertex);

    if (left > intervals[vertex - 1].right || right < intervals[vertex - 1].left) {
        return INT_MIN;
    }
    if (left <= intervals[vertex - 1].left && right >= intervals[vertex - 1].right) {
        return maximums[vertex - 1];
    }

    data_type left_maximum = getMaximumRecursive(left, right, 2 * vertex) + changeValues[2 * vertex - 1];
    data_type right_maximum = getMaximumRecursive(left, right, 2 * vertex + 1) + changeValues[2 * vertex];
    return std::fmax(left_maximum, right_maximum);
}


template <typename data_type, typename size_type>
void IntervalTreeSolver<data_type, size_type>::pushChangeValues(size_type vertex) {
    if (changeValues[vertex - 1] != 0) {
        data_type value = changeValues[vertex - 1];
        if (!intervals[vertex - 1].isElement()) {
            changeValues[2 * vertex - 1] += value;
            changeValues[2 * vertex] += value;
        }
        else {
            elements[intervals[vertex - 1].left] += value;
        }
        size_type length = intervals[vertex - 1].length();
        sums[vertex - 1] += value * length;
        minimums[vertex - 1] += value;
        maximums[vertex - 1] += value;

        changeValues[vertex - 1] = 0;
    }
}

template <typename data_type, typename size_type>
void IntervalTreeSolver<data_type, size_type>::updateFunctions(size_type vertex) {
    size_type length = intervals[2 * vertex].length();
    sums[vertex - 1] = sums[2 * vertex - 1] + sums[2 * vertex] +
        changeValues[2 * vertex - 1] * length +
        changeValues[2 * vertex] * length;
    minimums[vertex - 1] = std::fmin(minimums[2 * vertex - 1] + changeValues[2 * vertex - 1],
        minimums[2 * vertex] + changeValues[2 * vertex]);
    maximums[vertex - 1] = std::fmax(maximums[2 * vertex - 1] + changeValues[2 * vertex - 1],
        maximums[2 * vertex] + changeValues[2 * vertex]);

}



int main() {
    IntervalTreeSolver<int, unsigned> solver;

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    solver.solve(std::cin, std::cout);

    return 0;
}
