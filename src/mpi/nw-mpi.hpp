#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "helper.hpp"

#define MATCH 1
#define MISMATCH 0
#define GAP -1

#define SHIFTBITS 31
#define LONGBITS 63

#define COMMBUF_SIZE 64
