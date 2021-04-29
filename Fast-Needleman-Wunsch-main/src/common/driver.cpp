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
  
  long int size = (long int)(s1.size + 1) * (long int)(s2.size + 1);

  // allocate table and touch every page to ensure pretty allocation
  int* table = new int[size];
  for (long int i = 0; i < size; i += 1024) { table[i] = 0; }
  
  // run, time
  auto wallStart = std::chrono::system_clock::now();
  
  needlemanWunsch(s1, s2, table);
  
  auto wallDiff = std::chrono::system_clock::now() - wallStart;

  int wallMsec = std::chrono::duration_cast<std::chrono::milliseconds>(wallDiff).count();
  std::cout << wallMsec;

  // std::cout << "\nScore: " << table[size-1] << std::endl;

  delete table;

  return 0;
}
