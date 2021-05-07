CC = mpic++
COMMON = ../common
OMP = -L /usr/local/lib -fopenmp
OPT = -O3
FLAGS = -Wall $(OPT) -I $(COMMON)

all: hybrid-vert.e hybrid-horz.e

hybrid-vert.e: helper.o hybrid-vert.cpp $(COMMON)/nw-hybrid.hpp hybrid-vert-driver.cpp
	$(CC) $(FLAGS) $(OMP) -o hybrid-vert.e hybrid-vert.cpp helper.o

helper.o: $(COMMON)/helper.hpp $(COMMON)/helper.cpp
	$(CC) $(FLAGS) -c $(COMMON)/helper.cpp

clean:
	rm *.o; rm *.e
