#include <iostream>
#include <vector>

using std::vector;
using std::istream;
using std::ostream;

template <typename data_type, typename size_type>
class LCSSolver {
private:
	vector<data_type> firstSequence;
	vector<data_type> secondSequence;
	vector<data_type> answers;
	data_type answer;
	
private:
	void inputSequence(istream& inputStream, vector<data_type> *const inputSequence);
	data_type lengthOfLCS(data_type first, data_type second);
	void initializeArrayAnswers();
	
public:
	void inputData(istream& inputStream);
	void solve();
	void outputData(ostream& outputStream);

};

template <typename data_type, typename size_type>
void LCSSolver<data_type, size_type>::inputData(istream& inputStream) {
	inputSequence(inputStream, &firstSequence);
	inputSequence(inputStream, &secondSequence);
	
	initializeArrayAnswers();
}

template <typename data_type, typename size_type>
void LCSSolver<data_type, size_type>::inputSequence(istream& inputStream, vector<data_type> *const inputSequence) {
	size_type lengthData;
	inputStream >> lengthData;
	
	inputSequence->clear();
	inputSequence->resize(lengthData);
	
	for (size_type i = 0; i < lengthData; ++i) {
		inputStream >> inputSequence->at(i);
	}
}

template <typename data_type, typename size_type>
void LCSSolver<data_type, size_type>::initializeArrayAnswers() {
	size_type lengthArrayAnswers = firstSequence.size() * secondSequence.size();
	
	answers.clear();
	answers.resize(lengthArrayAnswers, -1);
}

template <typename data_type, typename size_type>
void LCSSolver<data_type, size_type>::outputData(ostream& outputStream) {
	outputStream << answer;
}

template <typename data_type, typename size_type>
void LCSSolver<data_type, size_type>::solve() {
	data_type sizeFirstSequence = firstSequence.size();
	data_type sizeSecondSequence = secondSequence.size();
	
	answer = lengthOfLCS(sizeFirstSequence, sizeSecondSequence);
}

template <typename data_type, typename size_type>
data_type LCSSolver<data_type, size_type>::lengthOfLCS(data_type indexFirst, data_type indexSecond) {
	if (indexFirst == 0 || indexSecond == 0)
		return 0;
	
	if (firstSequence[indexFirst - 1] == secondSequence[indexSecond - 1]) {
		int index = (indexSecond - 1) * firstSequence.size() + indexFirst - 1;
		
		if (answers[index] != -1)
			return answers[index];
		else
			return answers[index] = 1 + lengthOfLCS(indexFirst - 1, indexSecond - 1);
	}
	else {
		data_type answerNoSecond = 0;
		data_type answerNoFirst = 0;
		
		if (indexSecond != 1) {
			data_type indexNoSecond = (indexSecond - 2) * firstSequence.size() + indexFirst - 1;
			if (answers[indexNoSecond] != -1)
				answerNoSecond = answers[indexNoSecond];
			else
				answerNoSecond = answers[indexNoSecond] = lengthOfLCS(indexFirst, indexSecond - 1);
		}
		
		if (indexFirst != 1) {
			data_type indexNoFirst = (indexSecond - 1) * firstSequence.size() + indexFirst - 2;
			if (answers[indexNoFirst] != -1)
				answerNoFirst = answers[indexNoFirst];
			else
				answerNoFirst = answers[indexNoFirst] = lengthOfLCS(indexFirst - 1, indexSecond);
		}
		
		return std::max(answerNoFirst, answerNoSecond);
	}
}


int main() {
	LCSSolver<int, size_t> solver;
	solver.inputData(std::cin);
	solver.solve();
	solver.outputData(std::cout);
	
	return 0;
}

