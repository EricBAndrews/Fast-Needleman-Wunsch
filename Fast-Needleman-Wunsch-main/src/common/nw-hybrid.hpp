#ifndef NW_HYBRID_H
#define NW_HYBRID_H

#include <mpi.h>
#include <omp.h>

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "helper.hpp"

#define MATCH 1
#define MISMATCH 0
#define GAP -1

#define SHIFTBITS 31
#define LONGBITS 63

#define COMMBUF_SIZE 1920
// #define N 64
// #define M 128
#define N 96
#define M 400

#endif
