CC = mpic++
COMMON = ../common
OPT = -O3
FLAGS = -Wall $(OPT) -I $(COMMON)

all: nw-mpi.e

nw-mpi.e: helper.o nw-mpi.cpp $(COMMON)/nw-mpi.hpp $(COMMON)/mpidriver.cpp
	$(CC) $(FLAGS) -o nw-mpi.e nw-mpi.cpp helper.o

helper.o: $(COMMON)/helper.hpp $(COMMON)/helper.cpp
	$(CC) $(FLAGS) -c $(COMMON)/helper.cpp

clean:
	rm *.o; rm *.e
