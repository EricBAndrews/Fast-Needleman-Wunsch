#include "needleman-wunsch.hpp"

// rows, columns per block
// best for 4k blocks
// N = 128, M = 64 seems to work well

// takes in the array to be filled--don't care about the allocation time
void needlemanWunsch(dnaArray s1, dnaArray s2, int N, int M, int* t) {
  // convenience
  long int nRows = s2.size + 1;
  long int nCols = s1.size + 1;
  
#pragma omp parallel shared(t)
  {
    int a, b, c, m; // temps for max calculation
    
    // // populate table with INT_MIN
    long int tnum = omp_get_thread_num();
    long int nThreads = omp_get_num_threads();

#pragma omp barrier

    // populate first row
#pragma omp single nowait
    {
      t[0] = 0;
      for (long int i = 1; i < nCols; ++i) { t[i] = i * GAP; }
    }
    
    // populate first column
#pragma omp single
    {
      t[0] = 0;
      for (long int i = 1; i < nRows; ++i) {
        t[nCols*i] = i * GAP;
        t[nCols*i+1] = INT_MIN;
      }
    }

#pragma omp barrier
    int* depLocation; // above dependency
    
    // populate remaining table, one thread per row
    // do in blocks of N x M
    for (long int y = (tnum * N) + 1; y < nRows; y += (N * nThreads)) {
      // highest value i can iterate to
      long int iMax = (y+N)+(((nRows-(y+N))>>LONGBITS)&(nRows-(y+N)));
      
      // fill last row in block
      std::fill(&t[(nCols * (iMax - 1)) + 2], &t[(nCols * iMax) - 1], INT_MIN);
      for (long int x = 1; x < nCols; x += M) {
        // long int jMax = (x + M >= nCols) ? nCols : x + M;
        long int jMax = (x+M)+(((nCols-(x+M))>>LONGBITS)&(nCols-(x+M)));
        long int i = y;
        // for first row of block, need to wait on data dependencies
        for (long int j = x; j < jMax; ++j) {
          depLocation = &t[(i-1) * nCols + j];
          asm volatile ("loop:"
                        "movl (%[depLoc]), %%eax\n\t"
                        "cmpl $0x80000000, %%eax\n\t"
                        "je loop\n\t"
                        : // no output
                        : [depLoc]"r"(depLocation)
                        : "eax", "memory");
        
          m = -(s1.dna[j-1] == s2.dna[i-1]);
          a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
          b = t[((i-1) * nCols) + j] + GAP;
          c = t[(i * nCols) + j-1] + GAP;
        
          a = a - (((a - b) >> SHIFTBITS) & (a - b));
          a = a - (((a - c) >> SHIFTBITS) & (a - c));
          t[(i * nCols) + j] = a;
        }
        for (long int i = y + 1; i < iMax; ++i) {
          for (long int j = x; j < jMax; ++j) {
            m = -(s1.dna[j-1] == s2.dna[i-1]);
            a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
            b = t[((i-1) * nCols) + j] + GAP;
            c = t[(i * nCols) + j-1] + GAP;
        
            a = a - (((a - b) >> SHIFTBITS) & (a - b));
            a = a - (((a - c) >> SHIFTBITS) & (a - c));
            t[(i * nCols) + j] = a;
          }
        }
      }
    }
  }
  return;
}

#include "block-tuner.cpp"
