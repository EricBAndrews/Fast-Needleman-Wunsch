#include "nw-hybrid.hpp"

// rows, columns per block

// takes in the array to be filled--don't care about the allocation time
void needlemanWunsch(dnaArray s1, dnaArray s2, long int nCols,
                     int rank, int nProc, int* t) {
  // note: s1 across top, s2 down side
  
  // convenience
  long int nRows = s2.size + 1;
  
  // buffers for sending, receiving
  int* sendBuf = new int[COMMBUF_SIZE];
  int* recvBuf = new int[COMMBUF_SIZE];

  // first index of s1 in this table (last index of rank-1's work)
  long int start = ((s1.size + 1) / nProc) * rank - (rank > 0);

  // SETUP CODE
  // fills first row, declares various useful private variables
#pragma omp parallel
  {
    int a, b, c, m; // temps for calculation
    int* depLocation; // above dependency
    long int iMax;

    // threading info
    long int tnum = omp_get_thread_num();
    long int nThreads = omp_get_num_threads();

    // populate first row
#pragma omp single
    {
      for (long int i = 0; i < nCols; ++i) { t[i] = (i + start) * GAP; }
    }

// #pragma omp barrier
    
    // RANK 0
    // never receives; only sends to rank 1
    if (rank == 0) {
      // populate first column
#pragma omp single
      {
        for (long int i = 1; i < nRows; ++i) {
          t[nCols*i] = i * GAP;
          t[nCols*i+1] = INT_MIN;
        }
      }

      // printf("table set up\n");

      // this loop divides the work into blocks of height COMMBUF_SIZE
      for (long int h = 1; h < nRows; h += COMMBUF_SIZE) {
        // using OMP, fill this block of the table
        // largest value of y allowed--avoid running over table
        long int yMax = (h + COMMBUF_SIZE < nRows) ? h + COMMBUF_SIZE : nRows;

        // this loop divides the MPI block into smaller OMP blocks
        // y loops through blocks of height N
        for (long int y = h + (tnum * N); y < yMax; y += (N * nThreads)) {
          // bound i by min(yMax, y + N)
          // long int iMax = (y+N)+(((yMax-(y+N))>>LONGBITS)&(yMax-(y+N)));
          iMax = (yMax < y + N) ? yMax : y + N;

          // fill last row in block
          std::fill(&t[(nCols * (iMax - 1)) + 2], &t[(nCols * iMax) - 1], INT_MIN);

          // x loops through blocks of width M
          for (long int x = 1; x < nCols; x += M) {
            // bound j by min(nCols, x + M)
            // long int jMax = (x+M)+(((nCols-(x+M))>>LONGBITS)&(nCols-(x+M)));
            long int jMax = (nCols < x + M) ? nCols : x + M;

            // for first row of block, wait on data dependencies
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
                m = -(s1.dna[start+j-1] == s2.dna[i-1]);
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
        if (iMax == yMax) {
          for (long int i = h; i < iMax; ++i) { sendBuf[i-h] = t[(i*nCols) + nCols - 1]; }
          MPI_Send(sendBuf, yMax - h, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
      } // end h-loop
    } // end rank clause

    // RANK N-1
    // never sends; only receives from rank-1
    else if (rank == nProc - 1) {
      // populate first column
#pragma omp single
      {
        for (long int i = 1; i < nRows; ++i) { t[nCols*i+1] = INT_MIN; }
      }
      
      for (long int h = 1; h < nRows; h += COMMBUF_SIZE) {
        long int yMax = (h + COMMBUF_SIZE < nRows) ? h + COMMBUF_SIZE : nRows;

        // receive, copy into first column--do this only one one thread! cannot continue
        // until this is done.
#pragma omp single
        {
          MPI_Recv(recvBuf, COMMBUF_SIZE, MPI_INT, rank-1, rank-1,
                   MPI_COMM_WORLD, NULL);
          
          for (long int i = h; i < yMax; ++i) {
            t[(i*nCols)] = recvBuf[i-h];
          }
        }

        // loop blocking structure is identical to rank 0 from hereon out; omitting
        // detailed comments for neatness.
        for (long int y = h + (tnum * N); y < yMax; y += (N * nThreads)) {
          iMax = (yMax < y + N) ? yMax : y + N;
          std::fill(&t[(nCols * (iMax - 1)) + 2], &t[(nCols * iMax) - 1], INT_MIN);
          
          for (long int x = 1; x < nCols; x += M) {
            long int jMax = (nCols < x + M) ? nCols : x + M;
            
            // first row
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
        
              m = -(s1.dna[start+j-1] == s2.dna[i-1]);
              a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
              b = t[((i-1) * nCols) + j] + GAP;
              c = t[(i * nCols) + j-1] + GAP;
        
              a = a - (((a - b) >> SHIFTBITS) & (a - b));
              a = a - (((a - c) >> SHIFTBITS) & (a - c));
              t[(i * nCols) + j] = a;
            }

            // subsequent rows
            for (long int i = y + 1; i < iMax; ++i) {
              for (long int j = x; j < jMax; ++j) {
                m = -(s1.dna[start+j-1] == s2.dna[i-1]);
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
      } // end h-loop
    } // end rank clause

    // ALL OTHER RANKS
    else {
      // populate first column
#pragma omp single
      {
        for (long int i = 1; i < nRows; ++i) { t[nCols*i+1] = INT_MIN; }
      }
      
      for (long int h = 1; h < nRows; h += COMMBUF_SIZE) {
        long int yMax = (h + COMMBUF_SIZE < nRows) ? h + COMMBUF_SIZE : nRows;

        // receive, copy into first column--do this only one one thread! cannot continue
        // until this is done.
#pragma omp single
        {
          MPI_Recv(recvBuf, COMMBUF_SIZE, MPI_INT, rank-1, rank-1,
                   MPI_COMM_WORLD, NULL);
          
          for (long int i = h; i < yMax; ++i) {
            t[(i*nCols)] = recvBuf[i-h];
          }
        }

        // loop blocking structure is identical to rank 0 from hereon out; omitting
        // detailed comments for neatness.
        for (long int y = h + (tnum * N); y < yMax; y += (N * nThreads)) {
          iMax = (yMax < y + N) ? yMax : y + N;
          std::fill(&t[(nCols * (iMax - 1)) + 2], &t[(nCols * iMax) - 1], INT_MIN);
          
          for (long int x = 1; x < nCols; x += M) {
            long int jMax = (nCols < x + M) ? nCols : x + M;
            
            // first row
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
        
              m = -(s1.dna[start+j-1] == s2.dna[i-1]);
              a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
              b = t[((i-1) * nCols) + j] + GAP;
              c = t[(i * nCols) + j-1] + GAP;
        
              a = a - (((a - b) >> SHIFTBITS) & (a - b));
              a = a - (((a - c) >> SHIFTBITS) & (a - c));
              t[(i * nCols) + j] = a;
            }

            // subsequent rows
            for (long int i = y + 1; i < iMax; ++i) {
              for (long int j = x; j < jMax; ++j) {
                m = -(s1.dna[start+j-1] == s2.dna[i-1]);
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
        if (iMax == yMax) {
          for (long int i = h; i < iMax; ++i) { sendBuf[i-h] = t[(i*nCols) + nCols - 1]; }
          MPI_Send(sendBuf, yMax - h, MPI_INT, rank+1, rank, MPI_COMM_WORLD);
        }
      } // end h-loop
    } // end rank clause
  } // end parallel region
  return;
}

#include "hybrid-vert-driver.cpp"
