#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>

using std::vector;
using std::istream;
using std::ostream;

class ReverseKMPSolver {
private:
    vector<int> charSequence;
    vector<int> piSequence;
    vector<int> zSequence;

    int dataLength;

private:
    void makeString();
    void makeZSequence();

public:
    void solve();
    void inputData(istream& inputStream);
    void outputData(ostream& inputStream);

};


void ReverseKMPSolver::inputData(istream& inputStream) {
    inputStream >> dataLength;

    piSequence.resize(dataLength);

    for (int i = 0; i < dataLength; ++i) {
        inputStream >> piSequence[i];
    }
}

void ReverseKMPSolver::solve() {
    makeString();
    makeZSequence();
}

void ReverseKMPSolver::outputData(ostream& outputStream) {
    outputStream << zSequence[0];
    for (int i = 1; i < dataLength; ++i) {
        outputStream << ' ' << zSequence[i];
    }
}

void ReverseKMPSolver::makeString() {
    charSequence.resize(dataLength);

    charSequence[0] = 1;
    int nextSymbol = 2;

    for (int i = 1; i < dataLength; ++i) {
        if (piSequence[i] == 0) {
            charSequence[i] = nextSymbol++;
        }
        else {
            charSequence[i] = charSequence[piSequence[i] - 1];
        }
    }
}

void ReverseKMPSolver::makeZSequence() {
    zSequence.resize(dataLength, 0);

    int indexRightestZFunction = 0;
    int indexLastKnownSymbol = 0;

    for (int i = 1; i < dataLength; ++i) {
        if (indexLastKnownSymbol >= i) {
            zSequence[i] = std::min(
                indexLastKnownSymbol - i + 1,
                zSequence[i - indexRightestZFunction]);
        }
        while (i + zSequence[i] < dataLength && charSequence[i + zSequence[i]] == charSequence[zSequence[i]]) {
            ++zSequence[i];
        }
        if (i + zSequence[i] - 1 > indexLastKnownSymbol) {
            indexLastKnownSymbol = i + zSequence[i] - 1;
            indexRightestZFunction = i;
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
