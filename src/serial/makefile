CC = g++
OPT = -O3
COMMON = ../common
FLAGS = -Wall -std=c++11 $(OPT) -I $(COMMON)

all: serial.e

serial.e: serial.cpp $(COMMON)/driver.cpp helper.o
	$(CC) $(FLAGS) -o serial.e serial.cpp helper.o

helper.o: $(COMMON)/helper.cpp $(COMMON)/helper.hpp
	$(CC) $(FLAGS) -c $(COMMON)/helper.cpp

clean:
	rm *.o; rm *.e; rm -r *.dSYM
