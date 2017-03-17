#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using std::vector;
using std::string;
using std::istream;
using std::ostream;
using std::unordered_map;

const int ALPHABET_SIZE = 26;
const char MINIMAL_SYMBOL = 'a';


template <typename size_type>
class PatternMatcher {
private:
    struct Vertex {
        bool isTerminal;
        size_type parent;
        size_type symbolFromParent;
        size_type suffixLink;
        size_type dictionaryLink;
        vector<size_type> sons;
        vector<size_type> go;

        Vertex() : isTerminal(false), parent(-1), symbolFromParent(-1),
            suffixLink(-1), dictionaryLink(-1), sons(ALPHABET_SIZE, -1), go(ALPHABET_SIZE, -1)
        {}

    };

private:
    vector<string> texts;
    vector<string> patterns;
    size_type countTexts;
    size_type countPatterns;
    size_type textsSumLength;
    size_type patternsSumLength;

private:
    vector<Vertex> vertices;
    
    vector<size_type> countPatternInTexts;

public:
    void initializePrefixTree();
    void addText(const string& text);
    size_type get_link(size_type vertex);
    size_type go(size_type vertex, char symbol);

    void setAllSuffixLinks();
    void setAllDictionaryLinks();

public:
    void inputData(istream& inputStream);
    void solve();
    void outputData(ostream& outputStream);

};

template <typename size_type>
void PatternMatcher<size_type>::inputData(istream& inputStream) {
    inputStream >> countPatterns;
    patterns.resize(countPatterns);
    patternsSumLength = 0;
    for (size_type i = 0; i < countPatterns; ++i) {
        inputStream >> patterns[i];
        patternsSumLength += patterns[i].length();
    }

    inputStream >> countTexts;
    texts.resize(countTexts);
    textsSumLength = 0;
    for (size_type i = 0; i < countTexts; ++i) {
        inputStream >> texts[i];
        textsSumLength += texts[i].length();
    }
}

template <typename size_type>
void PatternMatcher<size_type>::solve() {
    initializePrefixTree();

    for (size_t i = 0; i < countTexts; ++i) {
        addText(texts[i]);
    }

    for (size_t i = 0; i < countPatterns; ++i) {
        size_type currentVertex = 0;
        for (size_t j = 0; j < patterns[i].length(); ++j) {
            currentVertex = go(currentVertex, patterns[i][j]);
        }
    }
}

template <typename size_type>
void PatternMatcher<size_type>::outputData(ostream& outputStream) {
    for (size_type i = 0; i < countPatternInTexts.size(); ++i) {
        outputStream << countPatternInTexts[i] << '\n';
    }
}

template <typename size_type>
void PatternMatcher<size_type>::initializePrefixTree() {
    Vertex emptyString;
    emptyString.sons.resize(ALPHABET_SIZE, -1);
    emptyString.go.resize(ALPHABET_SIZE, -1);
    emptyString.parent = emptyString.suffixLink = -1;

    vertices.push_back(emptyString);
}

template <typename size_type>
void PatternMatcher<size_type>::addText(const string& text) {
    size_type currentVertex = 0;

    for (size_type i = 0; i < text.length(); ++i) {
        char nextSymbol = text[i] - MINIMAL_SYMBOL;
        if (vertices[currentVertex].sons[nextSymbol] == -1) {
            Vertex newVertex;
            newVertex.parent = currentVertex;
            newVertex.symbolFromParent = nextSymbol;
            vertices[currentVertex].sons[nextSymbol] = vertices.size();
            vertices.push_back(newVertex);
        }
        currentVertex = vertices[currentVertex].sons[nextSymbol];
    }

    vertices[currentVertex].isTerminal = true;
}

template <typename size_type>
size_type PatternMatcher<size_type>::get_link(size_type vertex) {
    if (vertices[vertex].suffixLink == -1) {
        if (vertex == 0 || vertices[vertex].parent == 0) {
            vertices[vertex].suffixLink = 0;
        }
        else {
            vertices[vertex].suffixLink = go(
                get_link(vertices[vertex].parent),
                vertices[vertex].symbolFromParent);
        }
    }
    return vertices[vertex].suffixLink;
}

template <typename size_type>
size_type PatternMatcher<size_type>::go(size_type vertex, char symbol) {
    symbol = symbol - MINIMAL_SYMBOL;
    if (vertices[vertex].go[symbol] == -1) {
        if (vertices[vertex].sons[symbol] != -1) {
            vertices[vertex].go[symbol] = vertices[vertex].sons[symbol];
        }
        else {
            if (vertex == 0) {
                vertices[vertex].go[symbol] = 0;
            }
            else {
                vertices[vertex].go[symbol] = go(get_link(vertex), symbol);
            }
        }
    }
    return vertices[vertex].go[symbol];
}


int main() {
    PatternMatcher<long long> solver;

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    solver.inputData(std::cin);
    solver.solve();
    solver.outputData(std::cout);

    return 0;
}
