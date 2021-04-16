CC = g++-6
OPT = -O3
COMMON = ../common
FLAGS = -Wall -std=c++11 $(OPT) -I $(COMMON)
OMP = -fopenmp

all: idxarray-mt.e idxarray-emb-mt.e idxarray-mod-mt.e

idxarray-mt.e: idxarray-mt.cpp $(COMMON)/driver.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o idxarray-mt.e idxarray-mt.cpp helper.o

idxarray-emb-mt.e: idxarray-emb-mt.cpp $(COMMON)/driver.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o idxarray-emb-mt.e idxarray-emb-mt.cpp helper.o

idxarray-mod-mt.e: idxarray-mod-mt.cpp $(COMMON)/driver.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o idxarray-mod-mt.e idxarray-mod-mt.cpp helper.o

helper.o: $(COMMON)/helper.cpp $(COMMON)/helper.hpp
	$(CC) $(FLAGS) -c $(COMMON)/helper.cpp

clean:
	rm *.o; rm *.e; rm -r *.dSYM
