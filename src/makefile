CC = g++-6
OPT = -O3
FLAGS = -Wall -std=c++11 $(OPT)
OMP = -fopenmp

all: serial.e blocked-serial.e simple-mt.e \
sentinel-mt.e sentinel-parfill-mt.e sentinel-otf-mt.e sentinel-nofill-mt.e sentinel-otf-blocked-mt.e\
idxarray-mt.e idxarray-emb-mt.e idxarray-mod-mt.e

sentinels: sentinel-mt.e sentinel-parfill-mt.e sentinel-otf-mt.e sentinel-nofill-mt.e sentinel-otf-blocked-mt.e

serial.e: serial.cpp helper.o
	$(CC) $(FLAGS) -o serial.e serial.cpp helper.o

blocked-serial.e: blocked-serial.cpp helper.o
	$(CC) $(FLAGS) -o blocked-serial.e blocked-serial.cpp helper.o

simple-mt.e: simple-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o simple-mt.e simple-mt.cpp helper.o

sentinel-mt.e: sentinel-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-mt.e sentinel-mt.cpp helper.o

sentinel-parfill-mt.e: sentinel-parfill-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-parfill-mt.e sentinel-parfill-mt.cpp helper.o

sentinel-otf-mt.e: sentinel-otf-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-otf-mt.e sentinel-otf-mt.cpp helper.o

sentinel-otf-blocked-mt.e: sentinel-otf-blocked-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-otf-blocked-mt.e sentinel-otf-blocked-mt.cpp helper.o

sentinel-nofill-mt.e: sentinel-nofill-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o sentinel-nofill-mt.e sentinel-nofill-mt.cpp helper.o

idxarray-mt.e: idxarray-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o idxarray-mt.e idxarray-mt.cpp helper.o

idxarray-emb-mt.e: idxarray-emb-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o idxarray-emb-mt.e idxarray-emb-mt.cpp helper.o

idxarray-mod-mt.e: idxarray-mod-mt.cpp helper.o
	$(CC) $(FLAGS) $(OMP) -o idxarray-mod-mt.e idxarray-mod-mt.cpp helper.o

helper.o: helper.cpp helper.hpp
	$(CC) $(FLAGS) -c helper.cpp

clean:
	rm *.o; rm *.e; rm -r *.dSYM
