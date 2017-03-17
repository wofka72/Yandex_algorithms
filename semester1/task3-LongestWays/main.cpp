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
		
		vector<size_type> longestBrunches;
		size_type firstLongestBrunch;
		size_type secondRepeatLongestBrunch;
		size_type secondLongestBrunch;
		size_type longestWayWith;
		size_type longestWayWithout;
		
		
		size_type countSonsFirstLongestBrunches;
		size_type countSonsSecondLongestBrunches;
		
		size_type countFirstLongestBrunches;
		size_type countSecondLongestBrunches;
		size_type countLongestWaysWith;
		size_type countLongestWaysWithout;
				
	};

private:
	vector<Vertex> vertices;
	size_type countVertices;
	value_type rootIndex;
	
	size_type lengthLongestWay;
	size_type countLongestWays;
	
private:
	void setLengthAndCountInTree();
	void setParents();
	void setParentsDFS(const Vertex& vertex);
	
	void findLengthParameters(Vertex& vertex);
	void computeLengthParameters(Vertex& vertex);
	void findCountParameters(Vertex& vertex);
	void computeCountParameters(Vertex& vertex);
	
	
	void setLengthLongestBrunches(Vertex& vertex);
	void setLongestWayWithout(Vertex& vertex);
	
	void setCountSonsLongestBrunches(Vertex& vertex);
	void setCountLongestBrunches(Vertex& vertex);
	void setCountLongestWaysWith(Vertex& vertex);
	void setCountLongestWaysWithout(Vertex& vertex);
	
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
	outputStream << countLongestWays << std::endl;
}


template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::solve() {
	if (countVertices == 1) {
		lengthLongestWay = 0;
		countLongestWays = 0;
		return;
	}
	
	for (size_type i = 0; i < countVertices; ++i) {
		size_type size = vertices[i].adjacentVertices.size();
		vertices[i].longestBrunches.resize(size);
	}
	
	rootIndex = 1;
	setParents();
	
	findLengthParameters(vertices[rootIndex - 1]);
	findCountParameters(vertices[rootIndex - 1]);
	
	setLengthAndCountInTree();
}


template<typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::setParents() {
	vertices[rootIndex - 1].parent = rootIndex;
	setParentsDFS(vertices[rootIndex - 1]);
}

template<typename size_type, typename value_type>
inline void LongestWaysSolver<size_type, value_type>::setParentsDFS(const Vertex& vertex) {
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	
	for (size_type i = 0; i < countAdjacentVertices; ++i) {
		if (vertex.adjacentVertices[i] != vertex.parent) {
			vertices[vertex.adjacentVertices[i] - 1].parent = vertex.index;
			setParentsDFS(vertices[vertex.adjacentVertices[i] - 1]);
		}
	}
}


template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::findLengthParameters(Vertex& vertex) {
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	
	for (size_type i = 0; i < countAdjacentVertices; ++i) {
		if (vertex.adjacentVertices[i] != vertex.parent) {
			findLengthParameters(vertices[vertex.adjacentVertices[i] - 1]);
		}
	}
	
	computeLengthParameters(vertex);
}

template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::findCountParameters(Vertex& vertex) {
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	
	for (size_type i = 0; i < countAdjacentVertices; ++i) {
		if (vertex.adjacentVertices[i] != vertex.parent) {
			findCountParameters(vertices[vertex.adjacentVertices[i] - 1]);
		}
	}
	
	computeCountParameters(vertex);
}

template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::computeLengthParameters(Vertex& vertex) {
	if (vertex.adjacentVertices.size() == 1 && vertex.index != rootIndex) {
		vertex.firstLongestBrunch = 0;
		vertex.secondLongestBrunch = 0;
		vertex.secondRepeatLongestBrunch = 0;
		
		vertex.longestWayWith = 0;
		vertex.longestWayWithout = 0;
	}
	else {
		size_type countAdjacentVertices = vertex.adjacentVertices.size();
		for (size_type i = 0; i < countAdjacentVertices; ++i) {
			if (vertex.adjacentVertices[i] != vertex.parent) {
				vertex.longestBrunches[i] = 1 + vertices[vertex.adjacentVertices[i] - 1].firstLongestBrunch;
			}
		}
		
		setLengthLongestBrunches(vertex);
		vertex.longestWayWith = vertex.firstLongestBrunch + vertex.secondRepeatLongestBrunch;
		setLongestWayWithout(vertex);	
	}
}

template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::computeCountParameters(Vertex& vertex) {
	if (vertex.adjacentVertices.size() == 1 && vertex.index != rootIndex) {
		vertex.countFirstLongestBrunches = 1;
		vertex.countSecondLongestBrunches = 0;
		vertex.countLongestWaysWith = 1;
		vertex.countLongestWaysWithout = 0;
		vertex.countSonsSecondLongestBrunches = 0;
	}
	else {
		setCountLongestBrunches(vertex);
		setCountSonsLongestBrunches(vertex);		
		setCountLongestWaysWith(vertex);
		setCountLongestWaysWithout(vertex);
	}
}


template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::setLengthLongestBrunches(Vertex& vertex) {
	size_type firstMaximum = 0;
	size_type secondRepeatMaximum = 0;
	size_type secondMaximum = 0;
	
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	for (size_type i = 0; i < countAdjacentVertices; ++i) {
		if (vertex.adjacentVertices[i] != vertex.parent) {
			if (vertex.longestBrunches[i] > firstMaximum) {
				secondRepeatMaximum = firstMaximum;
				secondMaximum = firstMaximum;
				firstMaximum = vertex.longestBrunches[i];
			}
			else if (vertex.longestBrunches[i] == firstMaximum) {
				secondRepeatMaximum = firstMaximum;
			}
			else if (vertex.longestBrunches[i] > secondRepeatMaximum) {
				secondRepeatMaximum = vertex.longestBrunches[i];
				secondMaximum = vertex.longestBrunches[i];
			}
			else if (vertex.longestBrunches[i] > secondMaximum) {
				secondMaximum = vertex.longestBrunches[i];
			}
		}
	}
	
	vertex.firstLongestBrunch = firstMaximum;
	vertex.secondRepeatLongestBrunch = secondRepeatMaximum;
	vertex.secondLongestBrunch = secondMaximum;
}


template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::setLongestWayWithout(Vertex& vertex) {
	size_type longestWayWithout = 0;
	
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	for (size_type i = 0; i < countAdjacentVertices; ++i) {
		const Vertex& son = vertices[vertex.adjacentVertices[i] - 1];
		
		if (son.index != vertex.parent) {
			size_type potentialMaximum = std::max(son.longestWayWith, son.longestWayWithout);
			longestWayWithout = std::max(longestWayWithout, potentialMaximum);
		}
	}
	
	vertex.longestWayWithout = longestWayWithout;
}


template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::setCountLongestBrunches(Vertex& vertex) {
	size_type countFirstLB = 0;
	size_type countSecondLB = 0;
	
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	for (size_type i = 0; i < countAdjacentVertices; ++i) {
		const Vertex& son = vertices[vertex.adjacentVertices[i] - 1];
		
		if (son.index != vertex.parent) {
			if (vertex.longestBrunches[i] == vertex.firstLongestBrunch) {
				countFirstLB += son.countFirstLongestBrunches;
			}
			else if (vertex.longestBrunches[i] == vertex.secondLongestBrunch) {
				countSecondLB += son.countFirstLongestBrunches;
			}
		}
	}
	
	vertex.countFirstLongestBrunches = countFirstLB;
	vertex.countSecondLongestBrunches = countSecondLB;
}

template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::setCountSonsLongestBrunches(Vertex& vertex) {
	size_type countSonsFLB = 0;
	size_type countSonsSLB = 0;
	
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	for (size_type i = 0; i < countAdjacentVertices; ++i) {
		const Vertex& son = vertices[vertex.adjacentVertices[i] - 1];
		
		if (son.index != vertex.parent) {
			if (vertex.longestBrunches[i] == vertex.firstLongestBrunch) {
				++countSonsFLB;
			}
			else if (vertex.longestBrunches[i] == vertex.secondLongestBrunch) {
				++countSonsSLB;
			}
		}
	}
	
	vertex.countSonsFirstLongestBrunches = countSonsFLB;
	vertex.countSonsSecondLongestBrunches = countSonsSLB;
}

template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::setCountLongestWaysWith(Vertex& vertex) {	 
	size_type CLWWith = 0;
	
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	if (vertex.countSonsFirstLongestBrunches >= 2) {
		// (a_1 + ... + a_n)^2 - (a_1^2 + ... + a_n^2) = 2 * (a_1 * a_2 + ... + a_{n-1} * a_n)
		// and we want to compute (a_1 * a_2 + ... + a_{n - 1} * a_n
		size_type linear_sum = 0;
		size_type square_sum = 0;
		
		for (size_type i = 0; i < countAdjacentVertices; ++i) {
			if (vertex.adjacentVertices[i] != vertex.parent) {
				if (vertex.longestBrunches[i] == vertex.firstLongestBrunch) {
					size_type count = vertices[vertex.adjacentVertices[i] - 1].countFirstLongestBrunches;
					linear_sum += count;
					square_sum += count * count;
				}
			}
		}
		
		CLWWith = (linear_sum * linear_sum - square_sum) / 2;
	}
	else if (vertex.countSonsSecondLongestBrunches != 0) {
		size_type countSLB = 0;
		size_type countFLB;
		
		for (size_type i = 0; i < countAdjacentVertices; ++i) {
			const Vertex& son = vertices[vertex.adjacentVertices[i] - 1];
			
			if (son.index != vertex.parent) {
				if (vertex.longestBrunches[i] == vertex.firstLongestBrunch) {
					countFLB = son.countFirstLongestBrunches;
				}
				
				if (vertex.longestBrunches[i] == vertex.secondLongestBrunch) {
					countSLB += son.countFirstLongestBrunches;	
				}
			}
		}
		
		CLWWith = countFLB * countSLB;
	}
	else {
		for (size_type i = 0; i < countAdjacentVertices; ++i) {
			const Vertex& son = vertices[vertex.adjacentVertices[i] - 1];
			
			if (son.index != vertex.parent) {
				CLWWith += son.countFirstLongestBrunches;
			}
		}
	}
	
	vertex.countLongestWaysWith = CLWWith;
}

template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::setCountLongestWaysWithout(Vertex& vertex) {
	size_type CLWWithout = 0;
	
	size_type countAdjacentVertices = vertex.adjacentVertices.size();
	for (size_type i = 0; i < countAdjacentVertices; ++i) {
		const Vertex& son = vertices[vertex.adjacentVertices[i] - 1];
		
		if (son.index != vertex.parent) {
			if (vertex.longestWayWithout == son.longestWayWith) {
				CLWWithout += son.countLongestWaysWith;
			}
			
			if (vertex.longestWayWithout == son.longestWayWithout) {
				CLWWithout += son.countLongestWaysWithout;
			}
		}
	}
	
	vertex.countLongestWaysWithout = CLWWithout;
}

template <typename size_type, typename value_type>
void LongestWaysSolver<size_type, value_type>::setLengthAndCountInTree() {
	size_type LWWithRoot = vertices[rootIndex - 1].longestWayWith;
	size_type LWWithoutRoot = vertices[rootIndex - 1].longestWayWithout;
	
	lengthLongestWay = std::max(LWWithRoot, LWWithoutRoot);
	
	if (LWWithRoot > LWWithoutRoot) {
		countLongestWays = vertices[rootIndex - 1].countLongestWaysWith;
	}
	else if (LWWithRoot < LWWithoutRoot) {
		countLongestWays = vertices[rootIndex - 1].countLongestWaysWithout;	
	}
	else {
		countLongestWays = vertices[rootIndex - 1].countLongestWaysWith;
		countLongestWays += vertices[rootIndex - 1].countLongestWaysWithout;
	}
}


int main() {
	std::ios_base::sync_with_stdio(false);
	LongestWaysSolver<size_t, size_t> solver;
	solver.inputData(std::cin);
	solver.solve();
	solver.outputData(std::cout);
	
	return 0;
}
