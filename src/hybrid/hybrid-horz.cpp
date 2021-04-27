#include "nw-mpi.hpp"
#include <omp.h>

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
    int* depLocation;
    long int iMax;
    
    // populate first column
    for (long int i = 0; i < nRows; ++i) { t[i*nCols] = (i + start) * GAP; }

    // RANK 0
    // never receives; only sends to rank 1
    if (rank == 0) {
      // populate first row
      for (long int i = 0; i < nCols; ++i) { t[i] = i * GAP; }
    
      for (long int h = 1; h < nCols; h += COMMBUF_SIZE) {
        long int jMax = (h + COMMBUF_SIZE < nCols) ? h + COMMBUF_SIZE : nCols;
        for (long int i = 1; i < nRows; ++i) {
          for (long int j = h; j < jMax; ++j) {
            m = -(s1.dna[j-1] == s2.dna[start+i-1]);
            a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
            b = t[((i-1) * nCols) + j] + GAP;
            c = t[(i * nCols) + j-1] + GAP;
          
            a = a - (((a - b) >> SHIFTBITS) & (a - b));
            a = a - (((a - c) >> SHIFTBITS) & (a - c));
            t[(i * nCols) + j] = a;
          }
        }
        MPI_Send(&t[(nRows-1)*nCols+h], jMax - h, MPI_INT, 1, 0,
                 MPI_COMM_WORLD);
      }
    }

    // RANK N-1
    // never sends; only receives from rank-1
    else if (rank == nProc - 1) {
      for (long int h = 1; h < nCols; h += COMMBUF_SIZE) {
        long int jMax = (h + COMMBUF_SIZE < nCols) ? h + COMMBUF_SIZE : nCols;

        // receive into first row
        MPI_Recv(&t[h], jMax - h, MPI_INT, rank-1, rank-1,
                 MPI_COMM_WORLD, NULL);

        for (long int i = 1; i < nRows; ++i) {
          for (long int j = h; j < jMax; ++j) {
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

    // ALL OTHER RANKS
    // receive at the start, then send
    else {
      for (long int h = 1; h < nCols; h += COMMBUF_SIZE) {
        long int jMax = (h + COMMBUF_SIZE < nCols) ? h + COMMBUF_SIZE : nCols;

        // receive into first row
        MPI_Recv(&t[h], COMMBUF_SIZE, MPI_INT, rank-1, rank-1,
                 MPI_COMM_WORLD, NULL);

        for (long int i = 1; i < nRows; ++i) {
          for (long int j = h; j < jMax; ++j) {
            m = -(s1.dna[j-1] == s2.dna[start+i-1]);
            a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
            b = t[((i-1) * nCols) + j] + GAP;
            c = t[(i * nCols) + j-1] + GAP;
          
            a = a - (((a - b) >> SHIFTBITS) & (a - b));
            a = a - (((a - c) >> SHIFTBITS) & (a - c));
            t[(i * nCols) + j] = a;
          }
        }
        MPI_Send(&t[(nRows-1)*nCols+h], jMax-h, MPI_INT, rank+1,
                 rank, MPI_COMM_WORLD);
      }
    }
  }
  return;
}

#include "mpi-horz-driver.cpp"
