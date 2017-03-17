#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>
#include <exception>

using std::vector;
using std::stack;
using std::istream;
using std::ostream;
using std::max;
using std::exception;

template <typename data_type, typename request_type, typename size_type>
class QueueMaximumsFinder {
public:
    typedef typename vector<data_type>::iterator pointer_type;

public:
    QueueMaximumsFinder();
    ~QueueMaximumsFinder();

    void inputData(istream& inputStream);      
    void outputData(ostream& outputStream);
    void findMaximums();

private:
    vector<data_type> data;
    vector<request_type> requests;

    // When all requests will be processed we would get all maximums in this array.
    vector<data_type> maximums;

    stack<data_type> stackAdd;
    stack<data_type> stackDelete;
    stack<data_type> maximumsInStackAdd;
    stack<data_type> maximumsInStackDelete;

private:
    static const request_type REQUEST_ADD = 'R';
    static const request_type REQUEST_DELETE = 'L';
    
    // pointer r
    pointer_type rightPointer;

    data_type globalMaximum;
    data_type maximumOfStackAdd;
    data_type maximumOfStackDelete;

private:
    void inputArray(istream& inputStream);
    void inputRequests(istream& inputStream);

    void processRequestAdd();
    void processRequestDelete();
    void transfer();

};


template <typename data_type, typename request_type, typename size_type>
QueueMaximumsFinder<data_type, request_type, size_type>::QueueMaximumsFinder() {
    maximums.clear();
}

template <typename data_type, typename request_type, typename size_type>
QueueMaximumsFinder<data_type, request_type, size_type>::~QueueMaximumsFinder() {
    data.clear();
    requests.clear();
    maximums.clear();
}

template <typename data_type, typename request_type, typename size_type>
void QueueMaximumsFinder<data_type, request_type, size_type>::inputData(istream& inputStream) {
    inputArray(inputStream);
    inputRequests(inputStream);
}

template <typename data_type, typename request_type, typename size_type>
void QueueMaximumsFinder<data_type, request_type, size_type>::inputArray(istream& inputStream) {
    size_type arrayLength;
    inputStream >> arrayLength;

    data.resize(arrayLength);

    if (data.empty()) {
        return;
    }

    for (size_type i = 0; i < arrayLength; ++i) {
        inputStream >> data[i];
    }
}

template <typename data_type, typename request_type, typename size_type>
void QueueMaximumsFinder<data_type, request_type, size_type>::inputRequests(istream& inputStream) {
    size_type requestsCount;
    inputStream >> requestsCount;

    requests.resize(requestsCount);

    if (requests.empty()) {
        return;
    }

    for (size_type i = 0; i < requestsCount; ++i) {
        inputStream >> requests[i];
    }
}

template <typename data_type, typename request_type, typename size_type>
void QueueMaximumsFinder<data_type, request_type, size_type>::outputData(ostream& outputStream) {
    if (maximums.empty()) {
        return;
    }

    size_type maximumsCount = maximums.size();

    outputStream << maximums.front();
    for (size_type i = 1; i < maximumsCount; ++i) {
        outputStream << ' ' << maximums[i];
    }

    outputStream << std::endl;
}


template <typename data_type, typename request_type, typename size_type>
void QueueMaximumsFinder<data_type, request_type, size_type>::findMaximums() {
    if (data.empty()) {
        return;
    }
    
    rightPointer = data.begin();

    size_type requestsCount = requests.size();
    maximums.resize(requestsCount);

    data_type firstElem = *rightPointer;
    stackAdd.push(firstElem);
    maximumsInStackAdd.push(firstElem);
    maximumOfStackAdd = firstElem;
    globalMaximum = firstElem;

    for (size_type curRequestIndex = 0; curRequestIndex < requestsCount; ++curRequestIndex) {
        if (requests[curRequestIndex] == REQUEST_ADD) {
            processRequestAdd();
        }
        else if (requests[curRequestIndex] == REQUEST_DELETE) {
            processRequestDelete();
        }
        else {
            // This code provide compile error at the yandex.contest (but not in Visual Studio 2013),
            // so I change it.
            //exception ex("Unknown request!");
            //throw ex;
            throw "Unknown request!";
        }

        maximums[curRequestIndex] = globalMaximum;
    }
}

template <typename data_type, typename request_type, typename size_type>
void QueueMaximumsFinder<data_type, request_type, size_type>::processRequestAdd() {
    ++rightPointer;
    data_type newElem = *rightPointer;
    
    if (stackAdd.empty()) {
        maximumOfStackAdd = newElem;
    }
    else {
        maximumOfStackAdd = max(maximumOfStackAdd, newElem);
    }
    
    stackAdd.push(newElem);

    // Incomplete calculation of conditional expressions.
    // If stack is empty other conditions are not evaluated.
    while (!maximumsInStackAdd.empty() && newElem > maximumsInStackAdd.top()) {
        maximumsInStackAdd.pop();
    }
    maximumsInStackAdd.push(newElem);
    
    globalMaximum = max(globalMaximum, newElem);
}

template <typename data_type, typename request_type, typename size_type>
void QueueMaximumsFinder<data_type, request_type, size_type>::processRequestDelete() {
    if (stackDelete.empty()) {
        transfer();
    }

    data_type oldElem = stackDelete.top();
    stackDelete.pop();

    if (oldElem == maximumsInStackDelete.top()) {
        maximumsInStackDelete.pop();
    }
    
    if (!maximumsInStackDelete.empty()) {
        maximumOfStackDelete = maximumsInStackDelete.top();
        if (!maximumsInStackAdd.empty()) {
            globalMaximum = max(maximumOfStackDelete, maximumOfStackAdd);
        }
        else {
            globalMaximum = maximumOfStackDelete;
        }
    }
    else {
        globalMaximum = maximumOfStackAdd;
    }
}

template <typename data_type, typename request_type, typename size_type>
void QueueMaximumsFinder<data_type, request_type, size_type>::transfer() {
    while (!stackAdd.empty()) {
        data_type transferElem = stackAdd.top();
        stackAdd.pop();
        stackDelete.push(transferElem);
    }

    while (!maximumsInStackAdd.empty()) {
        data_type transferElem = maximumsInStackAdd.top();
        maximumsInStackAdd.pop();
        maximumsInStackDelete.push(transferElem);
    }
    
    maximumOfStackDelete = globalMaximum;
}

int main() {
    QueueMaximumsFinder<int, char, size_t> finder;

    try {
        finder.inputData(std::cin);
        finder.findMaximums();
        finder.outputData(std::cout);
    }
    catch (const char* error) {
        std::cerr << error << std::endl;
    }
    catch (exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    catch (...) {
        ; // may be do anything
    }

    system("pause");
    return 0;
}
