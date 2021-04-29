#include "nw-mpi.hpp"

// takes in the array to be filled--don't care about the allocation time
void needlemanWunsch(dnaArray s1, dnaArray s2, long int nCols,
                     int rank, int nProc, int* t) {
  // note: s1 across top, s2 down side
  
  // convenience
  long int nRows = s2.size + 1;
  int a, b, c, m; // temps for max calculation

  // buffers for sending, receiving
  int* sendBuf = new int[COMMBUF_SIZE];
  int* recvBuf = new int[COMMBUF_SIZE];

  // first index of s1 in this table (last index of rank-1's work)
  long int start = ((s1.size + 1) / nProc) * rank - (rank > 0);

  // populate first row
  for (long int i = 0; i < nCols; ++i) { t[i] = (i + start) * GAP; }

  // RANK 0
  // never receives; only sends to rank 1
  if (rank == 0) {
    // populate first column
    for (long int i = 0; i < nRows; ++i) { t[nCols*i] = i * GAP; }
    
    for (long int h = 1; h < nRows; h += COMMBUF_SIZE) {
      long int iMax = (h + COMMBUF_SIZE < nRows) ? h + COMMBUF_SIZE : nRows;
      for (long int i = h; i < iMax; ++i) {
        for (long int j = 1; j < nCols; ++j) {
          m = -(s1.dna[start+j-1] == s2.dna[i-1]);
          a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
          b = t[((i-1) * nCols) + j] + GAP;
          c = t[(i * nCols) + j-1] + GAP;
          
          a = a - (((a - b) >> SHIFTBITS) & (a - b));
          a = a - (((a - c) >> SHIFTBITS) & (a - c));
          t[(i * nCols) + j] = a;
        }
        sendBuf[i-h] = t[(i*nCols) + nCols-1];
      }
      MPI_Send(sendBuf, COMMBUF_SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
  }

  // RANK N-1
  // never sends; only receives from rank-1
  else if (rank == nProc - 1) {
    for (long int h = 1; h < nRows; h += COMMBUF_SIZE) {
      long int iMax = (h + COMMBUF_SIZE < nRows) ? h + COMMBUF_SIZE : nRows;

      // receive, copy into first column
      MPI_Recv(recvBuf, COMMBUF_SIZE, MPI_INT, rank-1, rank-1,
               MPI_COMM_WORLD, NULL);
      
      for (long int i = h; i < iMax; ++i) {
        t[(i*nCols)] = recvBuf[i-h];
      }

      for (long int i = h; i < iMax; ++i) {
        // t[(i*nCols)] = recvBuf[i-h];
        for (long int j = 1; j < nCols; ++j) {
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

  // ALL OTHER RANKS
  // receive at the start, then send
  else {
    for (long int h = 1; h < nRows; h += COMMBUF_SIZE) {
      long int iMax = (h + COMMBUF_SIZE < nRows) ? h + COMMBUF_SIZE : nRows;

      // receive, copy into first column
      MPI_Recv(recvBuf, COMMBUF_SIZE, MPI_INT, rank-1, rank-1,
               MPI_COMM_WORLD, NULL);
      for (long int i = h; i < iMax; ++i) {
        t[(i*nCols)] = recvBuf[i-h];
      }

      for (long int i = h; i < iMax; ++i) {
        // t[(i*nCols)] = recvBuf[i-h];
        for (long int j = 1; j < nCols; ++j) {
          m = -(s1.dna[start+j-1] == s2.dna[i-1]);
          a = t[((i-1) * nCols) + j-1] + ((m & MATCH) | (~m & MISMATCH));
          b = t[((i-1) * nCols) + j] + GAP;
          c = t[(i * nCols) + j-1] + GAP;
          
          a = a - (((a - b) >> SHIFTBITS) & (a - b));
          a = a - (((a - c) >> SHIFTBITS) & (a - c));
          t[(i * nCols) + j] = a;
        }
        sendBuf[i-h] = t[(i*nCols) + nCols-1];
      }
      MPI_Send(sendBuf, COMMBUF_SIZE, MPI_INT, rank+1, rank, MPI_COMM_WORLD);
    }
  }
  
  return;
}

#include "mpi-vert-driver.cpp"
