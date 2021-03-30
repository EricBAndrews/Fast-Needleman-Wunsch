#include "needleman-wunsch.hpp"

// takes in the array to be filled--don't care about the allocation time
void needlemanWunsch(dnaArray s1, dnaArray s2, int* t) {
  // convenience
  long int nRows = s2.size + 1;
  long int nCols = s1.size + 1;
  long int* idxArray;
  long int idxArrayLen;
  
#pragma omp parallel shared(t, idxArray)
  {     
    long int tnum = omp_get_thread_num();
    int a, b, c, m; // temps for max calculation
    long int nThreads = omp_get_num_threads();
    
#pragma omp single
    {
      idxArrayLen = nThreads * 2;
      idxArray = new long int[idxArrayLen];
    }

    // populate idxArray with 0s
#pragma omp single nowait
    {
      memset(idxArray, 0, idxArrayLen * sizeof(long int));
      idxArray[0] = nCols;
    }

    // populate first row
#pragma omp single nowait
    {
      t[0] = 0;
      for (long int i = 1; i < nCols; ++i) { t[i] = t[i-1] + GAP; }
    }
    
    // populate first column
#pragma omp single
    {
      t[0] = 0;
      for (long int i = 1; i < nRows; ++i) {
        t[nCols*i] = t[nCols*(i-1)] + GAP;
      }
    }
    
#pragma omp barrier
    
    long int depIdx;
    long int modIdx;
    long int* depLocation;
    
    // populate remaining table, one thread per row
    for (long int i = tnum + 1; i < nRows; i += nThreads) {
      modIdx = i % (nThreads*2);
      depIdx = (i + (idxArrayLen - 1)) % (idxArrayLen);
      depLocation = &idxArray[depIdx];
      for (long int j = 1; j < nCols; ++j) {
        // while (j > idxArray[depIdx]){ }
        // loops while above j less than previous thread's j
        // assembly so compiler doesn't "help" by removing "redundant" checks
        asm volatile ("loop:"
                      "movq (%[depLoc]), %%rax\n\t"
                      "cmpq %[j], %%rax\n\t"
                      "jl loop\n\t"
                      : // no output
                      : [depLoc]"r"(depLocation), [j]"r"(j)
                      : "rax", "memory");
        m = -(s1.dna[j-1] == s2.dna[i-1]);
        a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
        b = t[((i-1) * nCols) + j] + GAP;
        c = t[(i * nCols) + j-1] + GAP;
        
        a = a - (((a - b) >> SHIFTBITS) & (a - b));
        a = a - (((a - c) >> SHIFTBITS) & (a - c));
        t[(i * nCols) + j] = a;
        
        idxArray[modIdx] = j;
      }
      idxArray[depIdx] = 0; // above dependency done, reset
    }
  }
  return;
}

#include "driver.cpp"
