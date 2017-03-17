#include <iostream>
#include <fstream>
#include <vector>

using std::istream;
using std::ostream;
using std::vector;


template <typename size_type>
class Graph {
private:
    size_type graphSize;
    vector<vector<size_type>> vertices;


public:
    Graph() : graphSize(0) {
    }

    Graph(const Graph &otherGraph) {
        this->initialize(otherGraph.vertices);
    }

    Graph(const vector<vector<size_type>> &verticesOfGraph) {
        initialize(verticesOfGraph);
    }

    virtual void initialize(const vector<vector<size_type>> &verticesOfGraph) {
        graphSize = verticesOfGraph.size();
        vertices.resize(graphSize);

        for (size_type tail = 0; tail < graphSize; ++tail) {
            for (size_type j = 0; j < verticesOfGraph[tail].size(); ++j) {
                size_type head = verticesOfGraph[tail][j];
                vertices[tail].push_back(head);
            }
        }
    }

    const vector<size_type> &operator [](size_type vertex) const {
        return vertices[vertex];
    }

    size_type getSize() const {
        return graphSize;
    }
};


template <typename size_type>
class PEREPOLOH_Solver {
private:
    size_type countSons;
    Graph<size_type> initialGraph;
    vector<vector<size_type>> answer;

private:
    void dfs(const Graph<size_type> &graph);
    void dfsRecursive(const Graph<size_type> &graph,
                      vector <bool> &used, size_type vertex,
                      vector<bool> &endOfLoop, size_type &countLoops);

public:
    void inputData(istream &inputStream);
    void solve();
    void outputData(ostream &outputStream);

};


template <typename size_type>
void PEREPOLOH_Solver<size_type>::inputData(istream& inputStream) {
    inputStream >> countSons;
    vector<vector<size_type>> vertices(2 * countSons);
    
    for (size_type i = 0; i < countSons; ++i) {
        size_type countGirls;
        inputStream >> countGirls;
        
        size_type girl;
        inputStream >> girl;
        vertices[countSons + girl - 1].push_back(i);
        
        for (size_type j = 1; j < countGirls; ++j) {
            inputStream >> girl;
            vertices[i].push_back(countSons + girl - 1);
        }
    }

    initialGraph.initialize(vertices);
}

template <typename size_type>
void PEREPOLOH_Solver<size_type>::solve() {
    answer.resize(countSons);

    dfs(initialGraph);
}

template <typename size_type>
void PEREPOLOH_Solver<size_type>::outputData(ostream& outputStream) {
    for (size_type i = 0; i < countSons; ++i) {
        outputStream << answer[i].size();

        for (size_type j = 0; j < answer[i].size(); ++j) {
            outputStream << ' ' << (answer[i][j] - countSons + 1);
        }
        outputStream << '\n';
    }
}

template <typename size_type>
void PEREPOLOH_Solver<size_type>::dfs(const Graph<size_type> &graph) {
    vector<bool> used(countSons, false);
    size_type countLoops = 0;
    
    for (size_type i = 0; i < countSons; ++i) {
        if (!used[i]) {
            vector<bool> endOfLoop(countSons, false);
            dfsRecursive(graph, used, i, endOfLoop, countLoops);
        }
    }
}

template <typename size_type>
void PEREPOLOH_Solver<size_type>::dfsRecursive(const Graph<size_type> &graph,
                                               vector <bool> &used, size_type vertex,
                                               vector<bool> &endOfLoop, size_type &countLoops) {
    if (vertex < countSons) {
        used[vertex] = true;
    }

    for (size_type i = 0; i < graph[vertex].size(); ++i) {
        size_type nextVertex = graph[vertex][i];
        if (vertex < countSons) {
            dfsRecursive(graph, used, nextVertex, endOfLoop, countLoops);

            if (countLoops > 0) {
                answer[vertex].push_back(nextVertex);
            }

            if (endOfLoop[vertex] == true) {
                endOfLoop[vertex] = false;
                --countLoops;
            }
        }
        else {
            if (!used[nextVertex]) {
                dfsRecursive(graph, used, nextVertex, endOfLoop, countLoops);
            }
            else {
                if (nextVertex < countSons) {
                    endOfLoop[nextVertex] = true;
                    ++countLoops;
                }
            }
        }
    }
}


int main() {
    std::ifstream inputFileStream("input.txt");
    std::ofstream outputFileStream("output.txt");

    PEREPOLOH_Solver<int> solver;
    solver.inputData(inputFileStream);
    solver.solve();
    solver.outputData(outputFileStream);

    return 0;
}
