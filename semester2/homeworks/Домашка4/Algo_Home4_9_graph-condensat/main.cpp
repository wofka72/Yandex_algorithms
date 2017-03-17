#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <algorithm>

using std::vector;
using std::istream;
using std::ostream;
using std::copy;


template <typename size_type>
class Graph {
protected:
    size_type graphSize;
    vector<vector<size_type>> vertices;

protected:
    void transponate() {
        vector<vector<size_type>> transponatedVertices(graphSize);

        for (size_type tail = 0; tail < graphSize; ++tail) {
            for (size_type j = 0; j < vertices[tail].size(); ++j) {
                size_type head = vertices[tail][j];
                transponatedVertices[head].push_back(tail);
            }
        }
        vertices = transponatedVertices;
    }

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

    Graph<size_type> getTransponated() const {
        Graph<size_type> transponatedGraph(*this);
        transponatedGraph.transponate();
        return transponatedGraph;
    }

    const vector<size_type> &operator [](size_type vertex) const {
        return vertices[vertex];
    }

    size_type getSize() const {
        return graphSize;
    }
};

template <typename size_type>
class CondensationGraph {
private:
    size_type graphSize;
    size_type totalCountVertices;

    vector<vector<size_type>> superVertices;
    vector<vector<size_type>> vertices;

public:
    CondensationGraph() : graphSize(0), totalCountVertices(0) {
    }
    CondensationGraph(const vector<vector<size_type>> &verticesOfGraph,
                      const vector<vector<size_type>> & superVerticesOfGraph) {
        this->initialize(verticesOfGraph, superVerticesOfGraph);
    }
    CondensationGraph(const CondensationGraph &otherCondensationGraph) {
        this->initialize(otherCondensationGraph.vertices, otherCondensationGraph.superVertices);
    }

    virtual void initialize(const vector<vector<size_type>> &verticesOfGraph,
                            const vector<vector<size_type>> &superVerticesOfGraph) {
        graphSize = superVerticesOfGraph.size();

        totalCountVertices = 0;
        for (size_type superVertex = 0; superVertex < graphSize; ++superVertex) {
            totalCountVertices += superVerticesOfGraph[superVertex].size();
        }

        superVertices.assign(superVerticesOfGraph.begin(), superVerticesOfGraph.end());
        vertices.assign(verticesOfGraph.begin(), verticesOfGraph.end());
    }

    const vector<size_type> &operator [](size_type vertex) const {
        return vertices[vertex];
    }

    size_type getSize() const {
        return graphSize;
    }

    const vector<size_type> &getSuperVertex(size_type vertex) const {
        return superVertices[vertex];
    }

};

template <typename size_type>
class Task9Solver {
private:
    Graph<size_type> initialGraph;

    size_type minimumNeededToAddArcs;

private:
    vector<bool> getAchievableFromRootVertices(const Graph<size_type> &graph);
    void emptyDFS(const Graph<size_type> &graph, vector<bool> &used, size_type vertex);
    Graph<size_type> getUnachievableFromRootGraph(const Graph<size_type> &graph);

    vector<size_type> getOutputOrder(const Graph<size_type> &graph);
    void dfsOutputOrder(const Graph<size_type> &graph, vector<bool> &used,
                        vector<size_type> &outputOrder, size_type vertex);
    CondensationGraph<size_type> condensate(const Graph<size_type> &graph,
                                            const Graph<size_type> &transponatedGraph,
                                            const vector<size_type> &outputOrder);
    void dfsNextComponent(const Graph<size_type> &graph, vector<bool> &used,
                          vector<size_type> &component, size_type vertex);

    vector<bool> getAchievableFromOther(const CondensationGraph<size_type> &graph);

public:
    void inputData(istream &inputStream);
    void solve();
    void outputData(ostream &outputStream);

};


template <typename size_type>
void Task9Solver<size_type>::inputData(istream& inputStream) {
    size_type countVertices, countArcs;
    inputStream >> countVertices >> countArcs;
    vector<vector<size_type>> vertices(countVertices);

    size_type vertexTail, vertexHead;
    for (size_type i = 0; i < countArcs; ++i) {
        inputStream >> vertexTail >> vertexHead;
        vertices[vertexTail - 1].push_back(vertexHead - 1);
    }
    initialGraph.initialize(vertices);
}

template <typename size_type>
void Task9Solver<size_type>::solve() {
    Graph<size_type> unachievableFromRootGraph = getUnachievableFromRootGraph(initialGraph);
    Graph<size_type> transponatedUnachievableGraph = unachievableFromRootGraph.getTransponated();

    vector<size_type> outputOrder = getOutputOrder(unachievableFromRootGraph);
    CondensationGraph<size_type> condensationGraph =
        condensate(unachievableFromRootGraph, transponatedUnachievableGraph, outputOrder);

    vector<bool> achievableFromOther = getAchievableFromOther(condensationGraph);
    minimumNeededToAddArcs = 0;
    for (size_type vertex = 0; vertex < achievableFromOther.size(); ++vertex) {
        if (achievableFromOther[vertex] == false) {
            ++minimumNeededToAddArcs;
        }
    }
}

template <typename size_type>
void Task9Solver<size_type>::outputData(ostream& outputStream) {
    outputStream << minimumNeededToAddArcs;
}


template <typename size_type>
vector<bool> Task9Solver<size_type>::getAchievableFromRootVertices(const Graph<size_type> &graph) {
    vector<bool> used(graph.getSize(), false);

    const size_type ROOT = 0;
    emptyDFS(graph, used, ROOT);

    return used;
}

template <typename size_type>
void Task9Solver<size_type>::emptyDFS(const Graph<size_type> &graph, vector<bool> &used, size_type vertex) {
    used[vertex] = true;
    for (size_type j = 0; j < graph[vertex].size(); ++j) {
        size_type nextVertex = graph[vertex][j];
        if (!used[nextVertex]) {
            emptyDFS(graph, used, nextVertex);
        }
    }
}

template <typename size_type>
Graph<size_type> Task9Solver<size_type>::getUnachievableFromRootGraph(const Graph<size_type> &graph) {
    vector<bool> achievableVertices = getAchievableFromRootVertices(graph);

    vector<size_type> verticesDisplay(graph.getSize());
    size_type nextVertex = 0;
    size_type countUnachievableVertices = 0;
    for (size_type vertex = 0; vertex < graph.getSize(); ++vertex) {
        if (achievableVertices[vertex] == false) {
            verticesDisplay[vertex] = nextVertex++;
            ++countUnachievableVertices;
        }
        else {
            verticesDisplay[vertex] = -1;
        }
    }

    vector<vector<size_type>> unachievableVertices(countUnachievableVertices);

    size_type head, displayTail, displayHead;
    for (size_type tail = 0; tail < graph.getSize(); ++tail) {
        displayTail = verticesDisplay[tail];
        if (displayTail != -1) {
            for (size_type j = 0; j < graph[tail].size(); ++j) {
                head = graph[tail][j];
                if (achievableVertices[head] == false) {
                    displayHead = verticesDisplay[head];
                    unachievableVertices[displayTail].push_back(displayHead);
                }
            }
        }
    }

    Graph<size_type> unachievableGraph(unachievableVertices);
    return unachievableGraph;
}


template <typename size_type>
vector<size_type> Task9Solver<size_type>::getOutputOrder(const Graph<size_type> &graph) {
    vector<size_type> outputOrder;
    vector<bool> used(graph.getSize(), false);

    for (size_type vertex = 0; vertex < graph.getSize(); ++vertex) {
        if (!used[vertex]) {
            dfsOutputOrder(graph, used, outputOrder, vertex);
        }
    }
    return outputOrder;
}

template <typename size_type>
void Task9Solver<size_type>::dfsOutputOrder(const Graph<size_type> &graph, vector<bool> &used,
                                            vector<size_type> &outputOrder, size_type vertex) {
    used[vertex] = true;

    for (size_type j = 0; j < graph[vertex].size(); ++j) {
        size_type nextVertex = graph[vertex][j];
        if (!used[nextVertex]) {
            dfsOutputOrder(graph, used, outputOrder, nextVertex);
        }
    }
    outputOrder.push_back(vertex);
}

template <typename size_type>
CondensationGraph<size_type> Task9Solver<size_type>::condensate(const Graph<size_type> &graph,
                                                                const Graph<size_type> &transponatedGraph,
                                                                const vector<size_type> &outputOrder) {

    vector<vector<size_type>> superVerticesOfGraph;

    vector<bool> used(transponatedGraph.getSize(), false);
    vector<size_type> vertexToSuperVertex(transponatedGraph.getSize());

    vector<size_type> component;
    for (size_type i = 0, superVertex = 0; i < transponatedGraph.getSize(); ++i) {
        size_type vertex = outputOrder[transponatedGraph.getSize() - i - 1];
        if (!used[vertex]) {
            dfsNextComponent(transponatedGraph, used, component, vertex);
            superVerticesOfGraph.push_back(component);
            for (size_type i = 0; i < component.size(); ++i) {
                vertexToSuperVertex[component[i]] = superVertex;
            }
            ++superVertex;
        }
        component.clear();
    }

    vector<vector<size_type>> verticesOfGraph(superVerticesOfGraph.size());

    for (size_type tail = 0; tail < graph.getSize(); ++tail) {
        for (size_type j = 0; j < graph[tail].size(); ++j) {
            size_type head = graph[tail][j];
            size_type displayTail = vertexToSuperVertex[tail];
            size_type displayHead = vertexToSuperVertex[head];

            if (displayTail != displayHead) {
                verticesOfGraph[displayTail].push_back(displayHead);
            }
        }
    }

    CondensationGraph<size_type> condensationGraph(verticesOfGraph, superVerticesOfGraph);
    return condensationGraph;
}

template <typename size_type>
void Task9Solver<size_type>::dfsNextComponent(const Graph<size_type> &graph, vector<bool> &used,
                                              vector<size_type> &component, size_type vertex) {
    component.push_back(vertex);
    used[vertex] = true;

    for (size_type j = 0; j < graph[vertex].size(); ++j) {
        size_type nextVertex = graph[vertex][j];
        if (!used[nextVertex]) {
            dfsNextComponent(graph, used, component, nextVertex);
        }
    }
}

template <typename size_type>
vector<bool> Task9Solver<size_type>::getAchievableFromOther(const CondensationGraph<size_type> &graph) {
    vector<bool> achievableVertices(graph.getSize(), false);


    for (size_type tail = 0; tail < graph.getSize(); ++tail) {
        for (size_type j = 0; j < graph[tail].size(); ++j) {
            size_type head = graph[tail][j];
            achievableVertices[head] = true;
        }
    }
    return achievableVertices;
}


int main() {
    std::freopen("input.txt", "r", stdin);
    std::freopen("output.txt", "w", stdout);

    Task9Solver<unsigned int> solver;
    solver.inputData(std::cin);
    solver.solve();
    solver.outputData(std::cout);

    return 0;
}
