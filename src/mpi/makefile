CC = mpic++
OPTS = -Wall -O3

all: nw-mpi.e

nw-mpi.e: helper.o nw-mpi.cpp nw-mpi.hpp mpidriver.cpp
	$(CC) $(OPTS) -o nw-mpi.e nw-mpi.cpp helper.o

helper.o: helper.hpp helper.cpp
	$(CC) $(OPTS) -c helper.cpp

clean:
	rm *.o; rm *.e
