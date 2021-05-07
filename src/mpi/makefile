CC = mpic++
COMMON = ../common
OMP = -I /usr/local/incldue -L /usr/local/lib -fopenmp
OPT = -O3
FLAGS = -Wall $(OPT) -I $(COMMON) $(OMP)

all: mpi-vert.e mpi-horz.e

mpi-vert.e: helper.o mpi-vert.cpp $(COMMON)/nw-mpi.hpp mpi-vert-driver.cpp
	$(CC) $(FLAGS) -o mpi-vert.e mpi-vert.cpp helper.o

helper.o: $(COMMON)/helper.hpp $(COMMON)/helper.cpp
	$(CC) $(FLAGS) -c $(COMMON)/helper.cpp

clean:
	rm *.o; rm *.e
