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

int readNumber(istream &inputStream);
string readBinaryString(istream &inputStream);

class BarrowsWillerTransformator {
private:
    int initialStringIndex;

public:
    string decode(const string &transformedString, int initialStringIndex);
    int getInitialStringIndex() {
        return initialStringIndex;
    }
    void setInitialStringIndex(int index) {
        initialStringIndex = index;
    }
};

class MoveToFrontTransformator {
public:
    string decode(const string &transformedString);

};

class HaffmanCoder {
private:
    vector<char> decodeCodedText;
    int decodeCountSymbols;
    int decodeCountBits;
    
    unordered_map<string, char> displayCodeToSymbol;
    
    string decodeDecodedText;

public:
    void inputCodedData(istream &inputStream);
    void decode();
    string getDecodedText() {
        return decodeDecodedText;
    }
};


void HaffmanCoder::inputCodedData(istream &inputStream) {
    char symbol[2] = { ' ', '\0' };
    decodeCountSymbols = readNumber(inputStream);

    for (int i = 0; i < decodeCountSymbols; ++i) {
        string codeForSymbol = readBinaryString(inputStream);
        inputStream.read(symbol, C_SIZE);
        displayCodeToSymbol.insert(make_pair(codeForSymbol, symbol[0]));
        inputStream.read(symbol, C_SIZE);
    }
    
    decodeCountBits = readNumber(inputStream);
    
    inputStream.read(symbol, C_SIZE);
    while (!inputStream.eof()) {
        decodeCodedText.push_back(symbol[0]);
        inputStream.read(symbol, C_SIZE);
    }
}

void HaffmanCoder::decode() {
    string prefix;
    string bits;
    char symbol[2] = { ' ', '\0' };

    for (int bit = 0, symbolIndex = 0; bit < decodeCountBits; ++bit) {
        if (bit % 8 == 0) {
            char nextSymbol = decodeCodedText[symbolIndex];
            bits.erase();
            for (int i = 0; i < 8; ++i) {
                if (nextSymbol % 2 == 0) {
                    bits += "0";
                }
                else {
                    bits += "1";
                }
                nextSymbol >>= 1;
            }
            ++symbolIndex;
        }

        symbol[0] = bits[bit % 8];
        prefix.append(symbol);

        if (displayCodeToSymbol.find(prefix) != displayCodeToSymbol.end()) {
            symbol[0] = displayCodeToSymbol[prefix];
            decodeDecodedText.append(symbol);
            prefix.erase();
        }
    }
}


class Decompressor {
private:
    BarrowsWillerTransformator BWT;
    MoveToFrontTransformator MTFT;
    HaffmanCoder haffmanCoder;

    string decompressedText;

private:
    void initialize(istream &inputStream) {
        int index = readNumber(inputStream);
        char symbol[2] = { ' ', '\0' };

        BWT.setInitialStringIndex(index);
        haffmanCoder.inputCodedData(inputStream);
    }
    void actuallyDecompression() {
        haffmanCoder.decode();
        string decodedString = haffmanCoder.getDecodedText();
        string decodedFromMTFTString = MTFT.decode(decodedString);
        decompressedText = BWT.decode(decodedFromMTFTString, BWT.getInitialStringIndex());
    }
    void outputData(ostream& outputStream) {
        outputStream << decompressedText << '\n';
    }

public:
    void decompress(string inputFile, string outputFile);

};


string BarrowsWillerTransformator::decode(const string &transformedString, int initialStringIndex) {
    const int SIZE = transformedString.size();

    vector<int> countSymbol(ALPHABET_SIZE);
    vector<int> cummulateSumCountSymbol(ALPHABET_SIZE);
    vector<int> countEarlierSameSymbol(SIZE);
    vector<int> reverseTransfer(SIZE);
    vector<int> transfer(SIZE);

    for (int i = 0; i < SIZE; ++i) {
        int index = symbolToIndex[transformedString[i]];
        countEarlierSameSymbol[i] = countSymbol[index];
        ++countSymbol[index];
    }
    for (int i = 1; i < ALPHABET_SIZE; ++i) {
        cummulateSumCountSymbol[i] = cummulateSumCountSymbol[i - 1] + countSymbol[i - 1];
    }
    for (int i = 0; i < SIZE; ++i) {
        int index = symbolToIndex[transformedString[i]];
        reverseTransfer[i] = cummulateSumCountSymbol[index] + countEarlierSameSymbol[i];
        transfer[reverseTransfer[i]] = i;
    }

    string sortedString;
    char symbol[2] = { ' ', '\0' };
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        symbol[0] = ALLOWED_SYMBOLS[i];
        for (int j = 0; j < countSymbol[i]; ++j) {
            sortedString.append(symbol);
        }
    }

    string decodedString;
    int nextIndex = initialStringIndex;
    for (int i = 0; i < SIZE; ++i) {
        symbol[0] = sortedString[nextIndex];
        nextIndex = transfer[nextIndex];
        decodedString.append(symbol);
    }

    return decodedString;
}

string MoveToFrontTransformator::decode(const string &transformedString) {
    deque<int> encode;
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        encode.push_back(i);
    }
    string decodedString;
    char symbol[2] = { ' ', '\0' };

    for (int i = 0; i < transformedString.size(); ++i) {
        int codePoint = symbolToIndex[transformedString[i]];
        int index = encode[codePoint];
        char oneSymbol = ALLOWED_SYMBOLS[index];
        symbol[0] = oneSymbol;
        decodedString.append(symbol);
        encode.erase(encode.begin() + codePoint);
        encode.push_front(index);
    }
    return decodedString;
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
        symbol[0] = ' ';
        data.append(symbol);
    }
    return data;
}

string readBinaryString(istream &inputStream) {
    string codeString;
    char codeSymbol[2] = { ' ', '\0' };
    inputStream.read(codeSymbol, C_SIZE);

    while (codeSymbol[0] == '0' || codeSymbol[0] == '1') {
        codeString.append(codeSymbol);
        inputStream.read(codeSymbol, C_SIZE);
    }
    return codeString;
}

int readNumber(istream &inputStream) {
    int number = 0;
    char numberSymbol[2] = { ' ', '\0' };
    inputStream.read(numberSymbol, C_SIZE);

    while (numberSymbol[0] >= '0' && numberSymbol[0] <= '9') {
        number *= 10;
        number += (int)(numberSymbol[0] - '0');
        inputStream.read(numberSymbol, C_SIZE);
    }
    return number;
}

void Decompressor::decompress(string inputFile, string outputFile) {
    ifstream compressedInputStream(inputFile, std::ios::binary | std::ios::in);
    ofstream decompressedOutputStream(outputFile, std::ios::binary | std::ios::out);

    initialize(compressedInputStream);
    actuallyDecompression();
    outputData(decompressedOutputStream);
}


int main() {
    initialize();
    Decompressor decompressor;
    decompressor.decompress("input.txt", "output.txt");
    return 0;
}
