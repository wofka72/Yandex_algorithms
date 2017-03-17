#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>

using std::vector;
using std::istream;
using std::ostream;

class ReverseKMPSolver {
private:
    vector<int> dataSequence;
    vector<int> charSequence;
    vector<int> failureTable;

    bool isCorrectSequense;
    int dataLength;

private:
    void makeString();
    void makeFailureTable();
    void compareFailureTables();

public:
    void solve();
    void inputData(istream& inputStream);
    void outputData(ostream& inputStream);

};


void ReverseKMPSolver::inputData(istream& inputStream) {
    inputStream >> dataLength;

    dataSequence.resize(dataLength);

    for (int i = 0; i < dataLength; ++i) {
        inputStream >> dataSequence[i];
    }
}

void ReverseKMPSolver::solve() {
    makeString();
    makeFailureTable();
    compareFailureTables();
}

void ReverseKMPSolver::outputData(ostream& outputStream) {
    if (!isCorrectSequense) {
        outputStream << -1;
    }
    else {
        outputStream << charSequence[0];
        for (int i = 1; i < dataLength; ++i) {
            outputStream << ' ' << charSequence[i];
        }
    }
}

void ReverseKMPSolver::makeString() {
    charSequence.resize(dataLength);

    charSequence[0] = 1;
    int nextSymbol = 2;
    int maximumAvailible = 1;

    for (int i = 1; i < dataSequence.size(); ++i) {
        if (dataSequence[i] == 0) {
            charSequence[i] = nextSymbol++;
        }
        else {
            charSequence[i] = charSequence[dataSequence[i] - 1];
        }
    }
}

void ReverseKMPSolver::makeFailureTable() {
    failureTable.resize(dataLength);
    
    failureTable[0] = 0;

    for (int i = 1; i < dataLength; ++i) {
        if (charSequence[i] == charSequence[failureTable[i - 1]]) {
            failureTable[i] = failureTable[i - 1] + 1;
        }
        else {
            int smallerBorder = failureTable[i - 1]; // new length
            while (smallerBorder != 0 && charSequence[smallerBorder] != charSequence[i]) {
                smallerBorder = failureTable[smallerBorder - 1];
            }

            if (smallerBorder == 0) {
                if (charSequence[0] == charSequence[i]) {
                    failureTable[i] = 1;
                }
                else {
                    failureTable[i] = 0;
                }
            }
            else {
                failureTable[i] = smallerBorder + 1;
            }
        }
    }
}

void ReverseKMPSolver::compareFailureTables() {
    isCorrectSequense = true;
    for (int i = 0; i < dataLength; ++i) {
        if (dataSequence[i] != failureTable[i]) {
            isCorrectSequense = false;
            break;
        }
    }
}


int main() {
    ReverseKMPSolver solver;

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    solver.inputData(std::cin);
    solver.solve();
    solver.outputData(std::cout);

    return 0;
}
