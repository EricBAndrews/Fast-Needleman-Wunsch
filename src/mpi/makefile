CC = mpic++
COMMON = ../common
OPT = -O3
FLAGS = -Wall $(OPT) -I $(COMMON)

all: mpi-vert.e mpi-horz.e

mpi-vert.e: helper.o mpi-vert.cpp $(COMMON)/nw-mpi.hpp mpi-vert-driver.cpp
	$(CC) $(FLAGS) -o mpi-vert.e mpi-vert.cpp helper.o

mpi-horz.e: helper.o mpi-horz.cpp $(COMMON)/nw-mpi.hpp mpi-horz-driver.cpp
	$(CC) $(FLAGS) -o mpi-horz.e mpi-horz.cpp helper.o

mpi-horz-tune.e: helper.o mpi-horz-tune.cpp $(COMMON)/nw-mpi.hpp mpi-horz-tune-driver.cpp
	$(CC) $(FLAGS) -o mpi-horz-tune.e mpi-horz-tune.cpp helper.o

mpi-vert-tune.e: helper.o mpi-vert-tune.cpp $(COMMON)/nw-mpi.hpp mpi-vert-tune-driver.cpp
	$(CC) $(FLAGS) -o mpi-vert-tune.e mpi-vert-tune.cpp helper.o

helper.o: $(COMMON)/helper.hpp $(COMMON)/helper.cpp
	$(CC) $(FLAGS) -c $(COMMON)/helper.cpp

clean:
	rm *.o; rm *.e
