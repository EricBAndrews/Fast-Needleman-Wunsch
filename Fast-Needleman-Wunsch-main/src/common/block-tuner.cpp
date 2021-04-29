int main(int argc, char** argv){
  if (argc != 5) {
    std::cout << "error: incorrect number of arguments \
      (expected 4, got " << argc << ")" << std::endl;
    return 1;
  }

  dnaArray s1, s2;
  try {
    s1 = readSequence(argv[1]);
    s2 = readSequence(argv[2]);
  }

  catch (std::string e) {
    std::cout << "ERROR: no such file " << e << std::endl;
    return 1;
  }
  
  long int size = (long int)(s1.size + 1) * (long int)(s2.size + 1);

  // allocate table and touch every page to ensure pretty allocation
  int* table = new int[size];
  for (long int i = 0; i < size; i += 1024) { table[i] = 0; }

  int runs = 1;
  int N = atoi(argv[3]);
  int M = atoi(argv[4]);
  
  // run, time
  int sum = 0;
  for (int i = 0; i < runs; ++i) {
    auto wallStart = std::chrono::system_clock::now();
  
    needlemanWunsch(s1, s2, N, M, table);
  
    auto wallDiff = std::chrono::system_clock::now() - wallStart;

    sum += std::chrono::duration_cast<std::chrono::milliseconds>(wallDiff).count();
  }

  std::cout << sum / runs;

  // std::cout << "\nScore: " << table[size-1] << std::endl;

  delete table;
  
  return 0;
}
