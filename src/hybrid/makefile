CC = mpic++
COMMON = ../common
OMP = -L /usr/local/lib -fopenmp
OPT = -O3
FLAGS = -Wall $(OPT) -I $(COMMON)

# all: hybrid-vert.e hybrid-horz.e
all: hybrid-vert.e

hybrid-vert.e: helper.o hybrid-vert.cpp $(COMMON)/nw-hybrid.hpp hybrid-vert-driver.cpp
	$(CC) $(FLAGS) $(OMP) -o hybrid-vert.e hybrid-vert.cpp helper.o

hybrid-horz.e: helper.o hybrid-horz.cpp $(COMMON)/nw-hybrid.hpp hybrid-horz-driver.cpp
	$(CC) $(FLAGS) -o hybrid-horz.e hybrid-horz.cpp helper.o

hybrid-horz-tune.e: helper.o hybrid-horz-tune.cpp $(COMMON)/nw-hybrid.hpp hybrid-horz-tune-driver.cpp
	$(CC) $(FLAGS) -o hybrid-horz-tune.e hybrid-horz-tune.cpp helper.o

hybrid-vert-tune.e: helper.o hybrid-vert-tune.cpp $(COMMON)/nw-hybrid.hpp hybrid-vert-tune-driver.cpp
	$(CC) $(FLAGS) -o hybrid-vert-tune.e hybrid-vert-tune.cpp helper.o

helper.o: $(COMMON)/helper.hpp $(COMMON)/helper.cpp
	$(CC) $(FLAGS) -c $(COMMON)/helper.cpp

clean:
	rm *.o; rm *.e
