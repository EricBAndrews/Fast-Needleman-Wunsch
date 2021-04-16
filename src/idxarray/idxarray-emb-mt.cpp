#include "needleman-wunsch.hpp"

// takes in the array to be filled--don't care about the allocation time
void needlemanWunsch(dnaArray s1, dnaArray s2, int* t) {
  // convenience
  long int nRows = s2.size + 1;
  long int nCols = s1.size + 2;
  
#pragma omp parallel shared(t)
   {
     long int tnum = omp_get_thread_num();
     int a, b, c, m; // temps for max calculation
     long int nThreads = omp_get_num_threads();

    // populate first row
#pragma omp single nowait
    {
      t[0] = 0;
      for (long int i = 2; i < nCols; ++i) { t[i] = t[i-1] + GAP; }
    }
    
    // populate first column
#pragma omp single
    {
      t[1] = 0;
      for (long int i = 1; i < nRows; ++i) {
        t[nCols*i] = 0;
        t[nCols*i + 1] = t[nCols*(i-1) + 1] + GAP;
      }
      t[0] = nCols;
    }


    // #pragma omp master
    // {
    //   printTable(t, nRows, nCols);
    // }
    
#pragma omp barrier
    int* depLocation; // above dependency
    
    // populate remaining table, one thread per row
    for (long int i = tnum + 1; i < nRows; i += nThreads) {
      int& j = t[i*nCols];
      depLocation = &t[(i-1)*nCols];
      for (j = 2; j < nCols; ++j) {
        // while (j > *depLocation) { }
        // loops while above j less than previous thread's j
        // assembly so compiler doesn't "help" by removing "redundant" checks
        asm volatile ("loop:"
                      "movl (%[depLoc]), %%eax\n\t"
                      "cmpl %[j], %%eax\n\t"
                      "jle loop\n\t"
                      : // no output
                      : [depLoc]"r"(depLocation), [j]"r"(j)
                      : "eax", "memory");
        
        m = -(s1.dna[j-2] == s2.dna[i-1]);
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

#include "driver2.cpp"
