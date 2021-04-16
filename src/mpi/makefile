CC = mpic++
OPTS = -Wall

all: nw-mpi.e

nw-mpi.e: helper.o nw-mpi.cpp nw-mpi.hpp mpidriver.cpp
	$(CC) -o nw-mpi.e nw-mpi.cpp helper.o

helper.o: helper.hpp helper.cpp
	$(CC) -c helper.cpp

clean:
	rm *.o; rm *.e
