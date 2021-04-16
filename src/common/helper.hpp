#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct dnaArray {
  int size;
  int8_t* dna;
};

dnaArray readSequence(std::string fileName);

void printSequence(dnaArray toPrint);

void printTable(int* toPrint, int nRows, int nCols);

#endif
