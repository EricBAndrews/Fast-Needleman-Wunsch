#include "needleman-wunsch.hpp"

#define STRIPWIDTH 2048

// takes in the array to be filled--don't care about the allocation time
void needlemanWunsch(dnaArray s1, dnaArray s2, int* t) {
  // convenience
  long int nRows = s2.size + 1;
  long int nCols = s1.size + 1;
  int a, b, c, m; // temps for max calculation
  a = 0; // fix compiler warning
  int* inBuf = new int[nRows];
  int* outBuf = new int[nRows];
  
  // populate first row, column, inBuf
  // s1 across top, s2 down side
  for (long int i = 1; i < nCols; ++i) { t[i] = i* GAP; }
  for (long int i = 0; i < nRows; ++i) {
    outBuf[i] = i * GAP;
    t[(i*nCols)] = i * GAP;
  }
  
  
  // populate remaining table, row-major order
  for (long int s = 1; s < nCols; s += STRIPWIDTH) {
    long int jMax = (s+STRIPWIDTH)+(((nCols-(s+STRIPWIDTH))>>SHIFTBITS) &
                                    (nCols-(s+STRIPWIDTH)));
    // swap buffers--read from inBuf, write to outBuf
    int* tmp = inBuf;
    inBuf = outBuf;
    outBuf = tmp;
    inBuf[0] = (s-1) * GAP;
    
    for (long int i = 1; i < nRows; ++i) {
      // first spot--read from inBuf
      m = -(s1.dna[s-1] == s2.dna[i-1]);
      a = inBuf[i-1] + ((m & MATCH) | (~m & MISMATCH));
      b = t[((i-1) * nCols) + s] + GAP;
      c = inBuf[i] + GAP;

      // OPTIMIZATION: bitwise max 
      a = a - (((a - b) >> SHIFTBITS) & (a - b));
      a = a - (((a - c) >> SHIFTBITS) & (a - c));
      t[(i * nCols) + s] = a;

      // subsequent spots
      for (long int j = s + 1; j < jMax; ++j) {
        m = -(s1.dna[j-1] == s2.dna[i-1]);
        a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
        b = t[((i-1) * nCols) + j] + GAP;
        c = t[(i * nCols) + j-1] + GAP;

        // OPTIMIZATION: bitwise max 
        a = a - (((a - b) >> SHIFTBITS) & (a - b));
        a = a - (((a - c) >> SHIFTBITS) & (a - c));
        t[(i * nCols) + j] = a;
      }
      outBuf[i] = a;
    }
  }
  // printTable(t, nRows, nCols);
  return;
}

// main, timing code
#include "driver.cpp"
