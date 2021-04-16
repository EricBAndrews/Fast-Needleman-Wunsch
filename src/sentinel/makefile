CC = g++-6
OPT = -O3
COMMON = ../common
FLAGS = -Wall -std=c++11 $(OPT) -I $(COMMON)
OMP = -fopenmp

all:  sentinel-mt.e sentinel-parfill-mt.e sentinel-otf-mt.e sentinel-nofill-mt.e sentinel-otf-blocked-mt.e

sentinel-mt.e: sentinel-mt.cpp $(COMMON)/driver.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-mt.e sentinel-mt.cpp helper.o

sentinel-parfill-mt.e: sentinel-parfill-mt.cpp $(COMMON)/driver.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-parfill-mt.e sentinel-parfill-mt.cpp helper.o

sentinel-otf-mt.e: sentinel-otf-mt.cpp $(COMMON)/driver.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-otf-mt.e sentinel-otf-mt.cpp helper.o

sentinel-otf-blocked-mt.e: sentinel-otf-blocked-mt.cpp $(COMMON)/driver.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-otf-blocked-mt.e sentinel-otf-blocked-mt.cpp helper.o

sentinel-ob-tune.e: sentinel-ob-tune.cpp block-tuner.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-ob-tune.e sentinel-ob-tune.cpp helper.o

sentinel-nofill-mt.e: sentinel-nofill-mt.cpp $(COMMON)/driver.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-nofill-mt.e sentinel-nofill-mt.cpp helper.o

helper.o: $(COMMON)/helper.cpp $(COMMON)/helper.hpp
	$(CC) $(FLAGS) -c $(COMMON)/helper.cpp

clean:
	rm *.o; rm *.e; rm -r *.dSYM
