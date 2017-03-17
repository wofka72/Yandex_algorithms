#include <iostream>
#include <vector>

using std::istream;
using std::ostream;
using std::vector;

// size_type -- type for counters and the same variables
// value_type -- type for index of the vertices.
//				 This type is logically separated from size_type,
//				 but used as size_type in deferencing operation in the vector.
template <typename size_type, typename value_type>
class LongestWaysSolver {
private:
	struct Vertex {
		value_type index;
		
		vector<value_type> adjacentVertices;
		value_type parent;
		
		size_type maximumCountWith;
		size_type maximumCountWithout;
	
	};

private:
	vector<Vertex> vertices;
	size_type countVertices;
	value_type rootIndex;
	
	size_type maximumCountIndependent;
	
private:
	void setParents();
	void setParentsDFS(const Vertex& vertex);
	
	void findMaximumCountIndependentDFS(Vertex& vertex);
	void computeMaximumCountIndependent(Vertex& vertex);
	
public:
	void inputData(istream& inputStream);
	void solve();
	void outputData(ostream& outputStream);

};


template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::inputData(istream& inputStream) {
	size_type dataLength;
	inputStream >> dataLength;
	countVertices = dataLength;
	
	vertices.clear();
	vertices.resize(countVertices);
	
	for (size_type i = 0; i + 1 < countVertices; ++i) {
		value_type firstIndex, secondIndex;
		inputStream >> firstIndex >> secondIndex;
		
		vertices[firstIndex - 1].index = firstIndex;
		vertices[firstIndex - 1].adjacentVertices.push_back(secondIndex);
		vertices[secondIndex - 1].index = secondIndex;
		vertices[secondIndex - 1].adjacentVertices.push_back(firstIndex);
	}
}

template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::outputData(ostream& outputStream) {
	outputStream << maximumCountIndependent << std::endl;
}


template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::solve() {
	rootIndex = 1;
	setParents();
	
	findMaximumCountIndependentDFS(vertices[rootIndex - 1]);
	
	maximumCountIndependent = std::max(
				vertices[rootIndex - 1].maximumCountWith, vertices[rootIndex - 1].maximumCountWithout);
}


template<typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::setParents() {
	vertices[rootIndex - 1].parent = rootIndex;
	setParentsDFS(vertices[rootIndex - 1]);
}

template<typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::setParentsDFS(const Vertex& vertex) {
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	
	for (size_type i = 0; i < countAdjacentVertices; ++i) {
		if (vertex.adjacentVertices[i] != vertex.parent) {
			vertices[vertex.adjacentVertices[i] - 1].parent = vertex.index;
			setParentsDFS(vertices[vertex.adjacentVertices[i] - 1]);
		}
	}
}

template<typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::findMaximumCountIndependentDFS(Vertex& vertex) {
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	for (size_type i = 0; i < countAdjacentVertices; ++i) {
		Vertex& son = vertices[vertex.adjacentVertices[i] - 1];
		
		if (son.index != vertex.parent) {
			findMaximumCountIndependentDFS(son);
		}
	}
	
	computeMaximumCountIndependent(vertex);
}

template<typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::computeMaximumCountIndependent(Vertex& vertex) {
	if (vertex.adjacentVertices.size() == 1 && vertex.index != rootIndex) {
		vertex.maximumCountWith = 1;
		vertex.maximumCountWithout = 0;
	}
	else {
		size_type maxWith = 1;
		size_type maxWithout = 0;
		
		size_type countAdjacentVertices = vertex.adjacentVertices.size();
		for (size_type i = 0; i < countAdjacentVertices; ++i) {
			const Vertex& son = vertices[vertex.adjacentVertices[i] - 1];
			
			if (son.index != vertex.parent) {
				maxWith += son.maximumCountWithout;
				
				maxWithout += std::max(son.maximumCountWith, son.maximumCountWithout);
			}
		}
		
		vertex.maximumCountWith = maxWith;
		vertex.maximumCountWithout = maxWithout;
	}
}


int main() {
	std::ios_base::sync_with_stdio(false);
	LongestWaysSolver<unsigned int, unsigned int> solver;
	solver.inputData(std::cin);
	solver.solve();
	solver.outputData(std::cout);
	
	return 0;
}

