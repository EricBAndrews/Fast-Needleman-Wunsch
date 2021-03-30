CC = g++-6
OPT = -O3
FLAGS = -Wall -std=c++11 $(OPT) -g
OMP = -fopenmp

all: serial.e blocked-serial.e simple-mt.e sentinel-mt.e

serial.e: serial.cpp helper.o
	$(CC) $(FLAGS) -o serial.e serial.cpp helper.o

blocked-serial.e: blocked-serial.cpp helper.o
	$(CC) $(FLAGS) -o blocked-serial.e blocked-serial.cpp helper.o

simple-mt.e: simple-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o simple-mt.e simple-mt.cpp helper.o

sentinel-mt.e: sentinel-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-mt.e sentinel-mt.cpp helper.o

idxarray-mt.e: idxarray-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o idxarray-mt.e idxarray-mt.cpp helper.o

modIdxarray-mt.e: modIdxarray-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o modIdxarray-mt.e modIdxarray-mt.cpp helper.o

helper.o: helper.cpp helper.hpp
	$(CC) $(FLAGS) -c helper.cpp

clean:
	rm *.o; rm *.e; rm -r *.dSYM
