#include "needleman-wunsch.hpp"

// takes in the array to be filled--don't care about the allocation time
void needlemanWunsch(dnaArray s1, dnaArray s2, int* t) {
  // convenience
  long int nRows = s2.size + 1;
  long int nCols = s1.size + 1;
  
#pragma omp parallel shared(t)
  {
    int a, b, c, m; // temps for max calculation
    
    // // populate table with INT_MIN
    long int tnum = omp_get_thread_num();
    long int nThreads = omp_get_num_threads();
    long int blockSize = (nRows*nCols) / nThreads;
    long int iMax = tnum * blockSize + (
      tnum == omp_get_num_threads() - 1 ?
      blockSize + (nRows*nCols % blockSize) :
      blockSize);

    std::fill(&t[blockSize*tnum], &t[iMax], INT_MIN);

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
      }
    }

#pragma omp barrier
    int* depLocation; // above dependency
    
    // populate remaining table, one thread per row
    for (long int i = tnum + 1; i < nRows; i += nThreads) {
      for (long int j = 1; j < nCols; ++j) {
        // loops while above spot is sentinel value
        // assembly so compiler doesn't "help" by removing "redundant" checks
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
    }
  }
  return;
}

#include "driver.cpp"
