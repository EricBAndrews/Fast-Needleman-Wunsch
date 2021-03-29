int main(int argc, char** argv){
  if (argc != 3) {
    std::cout << "error: incorrect number of arguments \
      (expected 2, got " << argc << ")" << std::endl;
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

  // printSequence(s1);
  // printSequence(s2);
  
  unsigned long long int size = (unsigned long long)(s1.size + 1) * (unsigned long long)(s2.size + 1);
  // std::cout << size << std::endl;
  int* table = new int[size];

  // run, time
  clock_t cpuStart, cpuDiff;
  auto wallStart = std::chrono::system_clock::now();
  cpuStart = clock();
  
  needlemanWunsch(s1, s2, table);
  
  cpuDiff = clock() - cpuStart;
  auto wallDiff = std::chrono::system_clock::now() - wallStart;

  
  int cpuMsec = cpuDiff * 1000 / CLOCKS_PER_SEC;
  std::cout << "CPU:  " << cpuMsec << "ms" << std::endl;

  int wallMsec = std::chrono::duration_cast<std::chrono::milliseconds>(wallDiff).count();
  std::cout << "Wall: " << wallMsec << "ms" << std::endl;

  std::cout << "Score: " << table[size-1] << std::endl;

  free(table);
  
  return 0;
}
