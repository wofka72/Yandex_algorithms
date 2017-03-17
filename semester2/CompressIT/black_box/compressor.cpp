#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_map>

using namespace std;

const int C_SIZE = sizeof ((char)1);
const string ALLOWED_SYMBOLS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ()':,.!?\"";
const int ALPHABET_SIZE = ALLOWED_SYMBOLS.size();

unordered_map<char, int> symbolToIndex;

template <typename size_type>
class ISuffarayBuilder {
public:
    virtual vector<int> build(const string& s) = 0;
};

template <typename size_type>
class FastSuffixArrayBuilder : public ISuffarayBuilder<size_type> {
public:
    virtual vector<size_type> build(const string& initialString);
};

class BarrowsWillerTransformator {
private:
    int initialStringIndex;
public:
    string transform(const string &initialString);
    int getInitialStringIndex() {
        return initialStringIndex;
    }
};

class MoveToFrontTransformator {
public:
    string transform(const string &initialString);
};

struct Node {
    char symbol;
    int count;
    Node *leftSon;
    Node *rightSon;

    Node() : Node('#', 0, NULL, NULL) {
    }
    Node(const Node& other) : Node(other.symbol, other.count, other.leftSon, other.rightSon) {
    }
    Node(char s, int c) : Node(s, c, NULL, NULL) {
    }
    Node(char s, int c, Node *l, Node *r) : symbol(s), count(c), leftSon(l), rightSon(r) {
    }

    bool operator <(const Node &other) const {
        return other.count < count;
    }
};

class HaffmanCoder {
private:
    string text;

    vector<int> frequencyVocabulary;
    unordered_map<char, string> displaySymbolToCode;
    Node *codeTreeRoot;

    int totalCountBits;
    vector<char> codedText;

private:
    void makeFrequencyVocabulary();
    void makeCodeTree();
    void makeDisplaySymbolToCode(Node *currentNode, const string prefix);
    void codeText();

public:
    void code(const string &initialString) {
        totalCountBits = 0;
        text = initialString;

        makeFrequencyVocabulary();
        makeCodeTree();
        makeDisplaySymbolToCode(codeTreeRoot, "");
        codeText();
    }
    void outputCodedData(ostream &outputStream);

    vector<char> getCodedText() {
        return codedText;
    }
};

template <typename size_type>
vector<size_type> FastSuffixArrayBuilder<size_type>::build(const string& initialString) {
    string tandemString = initialString;

    size_type length = tandemString.size();
    vector<size_type> result(length);

    vector<size_type> class_num(length);
    vector<size_type> classes_count(std::max(length, (size_type)ALPHABET_SIZE));
    for (size_type i = 0; i < length; ++i) {
        class_num[i] = symbolToIndex[tandemString[i]];
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

    return result;
}


void HaffmanCoder::makeFrequencyVocabulary() {
    frequencyVocabulary.resize(ALPHABET_SIZE);
    for (int i = 0; i < text.size(); ++i) {
        int index = symbolToIndex[text[i]];
        ++frequencyVocabulary[index];
    }
}

void HaffmanCoder::makeCodeTree() {
    priority_queue<Node> symbolCounts;

    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (frequencyVocabulary[i] != 0) {
            symbolCounts.push(Node(ALLOWED_SYMBOLS[i], frequencyVocabulary[i]));
        }
    }

    while (symbolCounts.size() > 1) {
        Node firstNode = symbolCounts.top();
        symbolCounts.pop();
        Node secondNode = symbolCounts.top();
        symbolCounts.pop();

        Node parentNode('#', firstNode.count + secondNode.count);
        parentNode.leftSon = new Node(firstNode);
        parentNode.rightSon = new Node(secondNode);

        symbolCounts.push(parentNode);
    }
    codeTreeRoot = new Node(symbolCounts.top());
    symbolCounts.pop();
}

void HaffmanCoder::makeDisplaySymbolToCode(Node *currentNode, const string prefix) {
    if (currentNode->leftSon != NULL) {
        makeDisplaySymbolToCode(currentNode->leftSon, prefix + "0");
    }
    if (currentNode->rightSon != NULL) {
        makeDisplaySymbolToCode(currentNode->rightSon, prefix + "1");
    }

    if (currentNode->leftSon == NULL && currentNode->rightSon == NULL) {
        char symbol = currentNode->symbol;
        string codeNode = prefix;
        displaySymbolToCode.insert(make_pair(symbol, codeNode));
    }
}

void HaffmanCoder::codeText() {
    char symbol;
    string currentBits;

    for (int i = 0; i < text.size(); ++i) {
        currentBits.append(displaySymbolToCode[text[i]]);
        while (currentBits.size() >= 8) {
            totalCountBits += 8;
            symbol = 0;
            for (int bit = 0; bit < 8; ++bit) {
                if (currentBits[bit] == '1') {
                    symbol += 1 << bit;
                }
            }
            currentBits.erase(0, 8);
            codedText.push_back(symbol);

        }
    }

    if (currentBits.size() > 0) {
        totalCountBits += currentBits.size();
        symbol = 0;
        for (int bit = 0; bit < currentBits.size(); ++bit) {
            if (currentBits[bit] == '1') {
                symbol += 1 << bit;
            }
        }
        codedText.push_back(symbol);
        currentBits.clear();
    }
}

void HaffmanCoder::outputCodedData(ostream &outputStream) {
    outputStream << displaySymbolToCode.size() << '\n';
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (frequencyVocabulary[i] != 0) {
            char symbol = ALLOWED_SYMBOLS[i];
            outputStream << displaySymbolToCode[symbol] << ' ' << symbol << '\n';
        }
    }
    outputStream << totalCountBits << '\n';

    for (int i = 0; i < codedText.size(); ++i) {
        outputStream.put(codedText[i]);
    }
}

class Compressor {
private:
    BarrowsWillerTransformator BWT;
    MoveToFrontTransformator MTFT;
    HaffmanCoder haffmanCoder;

private:
    void actuallyCompression(const string &initialString) {
        string transformedByBWTString = BWT.transform(initialString);
        string transformedByMTFString = MTFT.transform(transformedByBWTString);
        haffmanCoder.code(transformedByMTFString);
    }
    void outputData(ostream &outputStream) {
        outputStream << BWT.getInitialStringIndex() << '\n';
        haffmanCoder.outputCodedData(outputStream);
    }

public:
    void compress(string inputFile, string outputFile);

};


string BarrowsWillerTransformator::transform(const string &initialString) {
    FastSuffixArrayBuilder<int> suffarrayBuilder;
    vector<int> suffarray = suffarrayBuilder.build(initialString);
    string transformedString;
    const int SIZE = suffarray.size();

    char symbol[2] = { ' ', '\0' };
    for (int i = 0; i < SIZE; ++i) {
        int index = (suffarray[i] + SIZE - 1) % SIZE;
        symbol[0] = initialString[index];
        transformedString.append(symbol);

        if (suffarray[i] == 0) {
            initialStringIndex = i;
        }
    }
    return transformedString;
}

string MoveToFrontTransformator::transform(const string &initialString) {
    deque<char> encode;
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        encode.push_back(ALLOWED_SYMBOLS[i]);
    }
    vector<int> transformedSequence(initialString.size());

    for (int i = 0; i < initialString.size(); ++i) {
        char symbol = initialString[i];
        for (int index = 0;; ++index) {
            if (encode[index] == symbol) {
                transformedSequence[i] = index;
                encode.erase(encode.begin() + index);
                encode.push_front(symbol);
                break;
            }
        }
    }

    string transformedString;
    char symbol[2] = { ' ', '\0' };
    for (int i = 0; i < transformedSequence.size(); ++i) {
        symbol[0] = ALLOWED_SYMBOLS[transformedSequence[i]];
        transformedString.append(symbol);
    }

    return transformedString;
}

void initialize() {
    for (int index = 0; index < ALPHABET_SIZE; ++index) {
        char symbol = ALLOWED_SYMBOLS[index];
        symbolToIndex.insert(make_pair(symbol, index));
    }
}

string readData(istream& inputStream) {
    string data;
    char symbol[2] = { ' ', '\0' };

    while (!inputStream.eof()) {
        inputStream.read(symbol, C_SIZE);
        if (symbol[0] == '\r' || symbol[0] == '\n') {
            break;
        }
        data.append(symbol);
    }
    return data;
}

void Compressor::compress(string inputFile, string outputFile) {
    ifstream aliceStream(inputFile, std::ios::binary | std::ios::in);
    ofstream compressedOutputStream(outputFile, std::ios::binary | std::ios::out);

    string aliceString = readData(aliceStream);

    actuallyCompression(aliceString);
    outputData(compressedOutputStream);
}


int main() {
    initialize();
    Compressor compressor;
    compressor.compress("input.txt", "compressed.txt");
    return 0;
}
