#include "nw-hybrid.hpp"

// takes in the array to be filled--don't care about the allocation time
void needlemanWunsch(dnaArray s1, dnaArray s2, long int nRows,
                     int rank, int nProc, int* t) {
  // note: s1 across top, s2 down side
  
  // convenience
  long int nCols = s1.size + 1;

  // first index of s2 in this table (last index of rank-1's work)
  long int start = ((s2.size + 1) / nProc) * rank - (rank > 0);

  // necessary for Isend
  // MPI_Request* dummy = new MPI_Request;

  #pragma omp parallel
  {
    int a, b, c, m; // temps for max calculation
    int* depLocation; // above dependency
    long int iMax = 0;
    long int jMax = 0;

    // threading info
    long int tnum = omp_get_thread_num();
    long int nThreads = omp_get_num_threads();
    
    // populate first column
    #pragma omp single
    {
      for (long int i = 0; i < nRows; ++i) {
        t[i*nCols] = (i + start) * GAP;
        t[nCols*i+1] = INT_MIN;
      }
    }

    // RANK 0
    // never receives; only sends to rank 1
    if (rank == 0) {
      // populate first row
      #pragma omp single
      {
        for (long int i = 0; i < nCols; ++i) { t[i] = i * GAP; }
      }
    
      for (long int h = 1; h < nCols; h += COMMBUF_SIZE) {

        for (long int y = (tnum * N) + 1; y < nRows; y += (N * nThreads)) {
          iMax = (nRows < y + N) ? nRows : y + N;
          jMax = (h + COMMBUF_SIZE < nCols) ? h + COMMBUF_SIZE : nCols;
          
          // this is maybe inefficient? fill last row in block as blocked by h
          // fills one extra to sync next block
          std::fill(&t[(nCols * (iMax - 1)) + h],
                    &t[(nCols * (iMax - 1)) + jMax + (jMax != nCols)],
                    INT_MIN);

          long int xMax = (h + COMMBUF_SIZE < nCols) ? h + COMMBUF_SIZE : nCols;
          for (long int x = h; x < xMax; x += M) {
            
            long int i = y;
            for (long int j = x; j < jMax; ++j) {
              depLocation = &t[(i-1) * nCols + j];
              asm volatile ("loopR0:"
                            "movl (%[depLoc]), %%eax\n\t"
                            "cmpl $0x80000000, %%eax\n\t"
                            "je loopR0\n\t"
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
            
            // fill the rest
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
        if (iMax == nRows) {
          MPI_Send(&t[(nRows-1)*nCols+h], jMax - h, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
        // #pragma omp barrier
      }
    }

    // RANK N-1
    // never sends; only receives from rank-1
    else if (rank == nProc - 1) {
      
      for (long int h = 1; h < nCols; h += COMMBUF_SIZE) {
        jMax = (h + COMMBUF_SIZE < nCols) ? h + COMMBUF_SIZE : nCols;

        #pragma omp single
        {
          MPI_Recv(&t[h], jMax - h, MPI_INT, rank-1, rank-1,
                   MPI_COMM_WORLD, NULL);
        }

        // #pragma omp barrier

        for (long int y = (tnum * N) + 1; y < nRows; y += (N * nThreads)) {
          iMax = (nRows < y + N) ? nRows : y + N;
          
          std::fill(&t[(nCols * (iMax - 1)) + h],
                    &t[(nCols * (iMax - 1)) + jMax + (jMax != nCols)],
                    INT_MIN);

          long int xMax = (h + COMMBUF_SIZE < nCols) ? h + COMMBUF_SIZE : nCols;
          for (long int x = h; x < xMax; x += M) {
            
            long int i = y;
            for (long int j = x; j < jMax; ++j) {
              depLocation = &t[(i-1) * nCols + j];
              asm volatile ("loopRL:"
                            "movl (%[depLoc]), %%eax\n\t"
                            "cmpl $0x80000000, %%eax\n\t"
                            "je loopRL\n\t"
                            : // no output
                            : [depLoc]"r"(depLocation)
                            : "eax", "memory");
        
              m = -(s1.dna[j-1] == s2.dna[start+i-1]);
              a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
              b = t[((i-1) * nCols) + j] + GAP;
              c = t[(i * nCols) + j-1] + GAP;
        
              a = a - (((a - b) >> SHIFTBITS) & (a - b));
              a = a - (((a - c) >> SHIFTBITS) & (a - c));
              t[(i * nCols) + j] = a;
            }
            
            // fill the rest
            for (long int i = y + 1; i < iMax; ++i) {
              for (long int j = x; j < jMax; ++j) {
                m = -(s1.dna[j-1] == s2.dna[start+i-1]);
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
    }

    // ALL OTHER RANKS
    // receive at the start, then send at the end of each h-loop
    else {
      
      for (long int h = 1; h < nCols; h += COMMBUF_SIZE) {
        jMax = (h + COMMBUF_SIZE < nCols) ? h + COMMBUF_SIZE : nCols;

        #pragma omp single
        {
          MPI_Recv(&t[h], jMax - h, MPI_INT, rank-1, rank-1,
                   MPI_COMM_WORLD, NULL);
        }

        for (long int y = (tnum * N) + 1; y < nRows; y += (N * nThreads)) {
          iMax = (nRows < y + N) ? nRows : y + N;
          
          // this is maybe inefficient? fill last row in block as blocked by h
          // fills one extra to sync next block
          std::fill(&t[(nCols * (iMax - 1)) + h],
                    &t[(nCols * (iMax - 1)) + jMax + (jMax != nCols)],
                    INT_MIN);

          long int xMax = (h + COMMBUF_SIZE < nCols) ? h + COMMBUF_SIZE : nCols;
          for (long int x = h; x < xMax; x += M) {
            
            long int i = y;
            for (long int j = x; j < jMax; ++j) {
              depLocation = &t[(i-1) * nCols + j];
              asm volatile ("loop:"
                            "movl (%[depLoc]), %%eax\n\t"
                            "cmpl $0x80000000, %%eax\n\t"
                            "je loop\n\t"
                            : // no output
                            : [depLoc]"r"(depLocation)
                            : "eax", "memory");
        
              m = -(s1.dna[j-1] == s2.dna[start+i-1]);
              a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
              b = t[((i-1) * nCols) + j] + GAP;
              c = t[(i * nCols) + j-1] + GAP;
        
              a = a - (((a - b) >> SHIFTBITS) & (a - b));
              a = a - (((a - c) >> SHIFTBITS) & (a - c));
              t[(i * nCols) + j] = a;
            }
            
            // fill the rest
            for (long int i = y + 1; i < iMax; ++i) {
              for (long int j = x; j < jMax; ++j) {
                m = -(s1.dna[j-1] == s2.dna[start+i-1]);
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
        if (iMax == nRows) {
          MPI_Send(&t[(nRows-1)*nCols+h], jMax - h, MPI_INT, rank+1, rank,
                   MPI_COMM_WORLD);
        }
      }
    }
  }
  
  return;
}

#include "hybrid-horz-driver.cpp"
