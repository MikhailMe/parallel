#pragma once

#ifndef PARALLEL_HELPER_H
#define PARALLEL_HELPER_H

#include <mpi.h>

#include "matrix/matrix.h"



typedef struct
{
    unsigned long **res;
    unsigned long **mat1;
    unsigned long **mat2;
    unsigned long to_line;
    unsigned long from_line;
    unsigned long mat1_cols;
    unsigned long mat2_cols;
} Data;

int counter = 0;



#endif //PARALLEL_HELPER_H