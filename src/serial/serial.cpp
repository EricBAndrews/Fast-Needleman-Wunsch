#include "needleman-wunsch.hpp"

// takes in the array to be filled--don't care about the allocation time
void needlemanWunsch(dnaArray s1, dnaArray s2, int* t) {
  // convenience
  long int nRows = s2.size + 1;
  long int nCols = s1.size + 1;
  int a, b, c, m; // temps for max calculation
  
  // populate first row, column
  // s1 across top, s2 down side
  t[0] = 0;
  

  // populate first row and column
  for (long int i = 1; i < nCols; ++i) { t[i] = t[i-1] + GAP; }
  for (long int i = 1; i < nRows; ++i) { t[nCols*i] = t[nCols*(i-1)] + GAP; }
  
  
  // populate remaining table, row-major order
  for (long int i = 1; i < nRows; ++i) {
    for (long int j = 1; j < nCols; ++j) {
      m = -(s1.dna[j-1] == s2.dna[i-1]);
      a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
      b = t[((i-1) * nCols) + j] + GAP;
      c = t[(i * nCols) + j-1] + GAP;

      // OPTIMIZATION: bitwise max 
      a = a - (((a - b) >> SHIFTBITS) & (a - b));
      a = a - (((a - c) >> SHIFTBITS) & (a - c));
      t[(i * nCols) + j] = a;
    }
  }
  
  return;
}

// main, timing code
#include "driver.cpp"
