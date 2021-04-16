#include "helper.hpp"

dnaArray readSequence(std::string fileName) {
  std::ifstream fin(fileName, std::ifstream::in);
  if (!fin.is_open()) { throw fileName; }
  
  std::vector<char>* inVec = new std::vector<char>;
  
  char c = fin.get();
  while (fin.good()) {
    inVec->push_back(c);
    c = fin.get();
  }

  int8_t* retArr = new int8_t[inVec->size()];
  for (unsigned int i = 0; i < inVec->size(); ++i) {
    retArr[i] = (int8_t)(inVec->at(i));
  }

  dnaArray ret;
  ret.dna = retArr;
  ret.size = inVec->size();
  
  return ret;
}

void printSequence(dnaArray toPrint) {
  char bases[5] = { '-', 'A', 'T', 'G', 'C' };
  for (int i = 0; i < toPrint.size; ++i) {
    std::cout << bases[toPrint.dna[i]];
  }
  std::cout << std::endl;
  return;
}

void printTable(int* toPrint, int nRows, int nCols) {
  for (int i = 0; i < nRows; ++i) {
    for (int j = 0; j < nCols; ++j) {
      std::cout << toPrint[j + (i * nCols)] << "\t";
    }
    std::cout << std::endl;
  }
  return;
}
