#include "needleman-wunsch.hpp"

#define STRIP_SIZE 8000

// takes in the array to be filled--don't care about the allocation time
void needlemanWunsch(dnaArray s1, dnaArray s2, int* t) {
  // convenience
  int nRows = s2.size + 1;
  int nCols = s1.size + 1;
  int a, b, c, m; // temps for max calculation
  
  // populate first row, column
  // s1 across top, s2 down side
  t[0] = 0;
  

  // OPTIMIZATION: single loop to populate; reduce branching
  if (nCols < nRows) {
    for (int i = 1; i < nCols; ++i) {
      t[i] = t[i-1] + GAP;
      t[i * nCols] = t[(i-1) * nCols] + GAP;
    }
    for (int i = nCols; i < nRows; ++i) {
      t[i * nCols] = t[(i-1) * nCols] + GAP;
    }
  }
  else {
    for (int i = 1; i < nRows; ++i) {
      t[i] = t[i-1] + GAP;
      t[i * nCols] = t[(i-1) * nCols] + GAP;
    }
    for (int i = nRows; i < nCols; ++i) {
      t[i] = t[i-1] + GAP;
    }
  }


  // TODO: use previous array to store last values; make this
  // look like paper dictates
  
  // populate remaining table in strips of size STRIP_SIZE
  for (int i = 1; i < nCols; i += STRIP_SIZE) { // start of each strip
    // std::cout << i << std::endl;
    for (int j = 1; j < nRows; ++j) { // for each row in strip
      // a: i + STRIP_SIZE
      // b: nCols
      int kMax = nCols + ((((i + STRIP_SIZE) - nCols) >> SHIFTBITS) & ((i + STRIP_SIZE) - nCols));
      for (int k = i; k < kMax; ++k) { // for each col in strip
        m = -(s1.dna[k-1] == s2.dna[j-1]);
        a = t[((j-1) * nCols) + k-1] + ((m & MATCH) | (~m & MISMATCH));
        b = t[((j-1) * nCols) + k] + GAP;
        c = t[(j * nCols) + k-1] + GAP;

        a = a - (((a - b) >> SHIFTBITS) & (a - b));
        a = a - (((a - c) >> SHIFTBITS) & (a - c));
        t[(j * (s1.size + 1)) + k] = a;
      }
    }
  }
  
  return;
}

#include "driver.cpp"
