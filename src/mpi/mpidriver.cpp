#include <chrono>
#include <iostream>

// TODO
// Try horizontal strips over vertical
//  (tune omp to hande that better)
//  can use memcpy for communication
//  might not even need a dedicated buffer--can use the table itself
//  this would allow us to use Isend...

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("error: incorrect number of arguments (expected 2, got %i)\n",
           argc - 1);
    return 1;
  }
      
  MPI_Init(NULL, NULL);
  int nProc, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &nProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  dnaArray s1, s2;
  try {
    s1 = readSequence(argv[1]);
    s2 = readSequence(argv[2]);
  }

  catch (std::string e) {
    std::cout << "ERROR: no such file " << e << std::endl;
    printf("ERROR: file not found\n");
    return 1;
  }

  long int nCols = ((s1.size + 1) / nProc) + (rank > 0);
  if (rank == nProc - 1) { nCols += (s1.size + 1) % nProc; }
  long int size = nCols * (long int)(s2.size + 1);
  int* table = new int[size];
  for (long int i = 0; i < size; i += 1024) { table[i] = 0; }

  // run, time
  auto wallStart = std::chrono::system_clock::now();
  
  needlemanWunsch(s1, s2, nCols, rank, nProc, table);

  auto wallDiff = std::chrono::system_clock::now() - wallStart;
  int wallMsec = std::chrono::duration_cast<std::chrono::milliseconds>(wallDiff).count();

  MPI_Barrier(MPI_COMM_WORLD);
  
  if (rank == nProc - 1) {
    printf("final score: %i\n", table[size-1]);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  
  printf("P%i | time: %ims\n", rank, wallMsec);
  
  MPI_Finalize();
  
  return 0;
}
