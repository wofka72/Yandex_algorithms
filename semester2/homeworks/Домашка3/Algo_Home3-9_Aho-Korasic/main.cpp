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
        size_type symbolParent;
        size_type suffixLink;
        size_type dictionaryLink;
        vector<size_type> sons;
        vector<size_type> suffixes;
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
}

template <typename size_type>
void PatternMatcher<size_type>::outputData(ostream& outputStream) {
    for (size_type i = 0; i < countPatterns; ++i) {
        outputStream << countPatternInTexts[i] << '\n';
    }
}

template <typename size_type>
void PatternMatcher<size_type>::initializePrefixTree() {
    Vertex emptyString;
    emptyString.sons.resize(ALPHABET_SIZE);
    emptyString.suffixes.resize(ALPHABET_SIZE);
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
            newVertex.sons.resize(ALPHABET_SIZE);
            newVertex.suffixes.resize(ALPHABET_SIZE);
            newVertex.suffixLink = -1;
            newVertex.parent = currentVertex;
            newVertex.symbolParent = nextSymbol;
            newVertex.sons[nextSymbol] = vertices.size();
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
            vertices[vertex].suffixLink = go(get_link(vertices[vertex].parent), vertices[vertex].symbolParent);
        }
    }
   return vertices[vertex].suffixLink;
}

template <typename size_type>
size_type PatternMatcher<size_type>::go(size_type vertex, char symbol) {
    if (vertices[vertex].suffixes[symbol] == -1)
        if (vertices[vertex].sons[symbol] != -1)
            vertices[vertex].suffixes[symbol] = vertices[vertex].sons[symbol];
        else
            vertices[vertex].suffixes[symbol] = vertex == 0 ? 0 : go(get_link(v), c);
    return vertices[vertex].suffixes[symbol];
}


int main() {
    PatternMatcher<long long> solver;
    
    solver.inputData(std::cin);
    solver.solve();
    solver.outputData(std::cout);

    return 0;
}
