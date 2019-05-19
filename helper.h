#pragma once

#ifndef PARALLEL_HELPER_H
#define PARALLEL_HELPER_H

#include <chrono>
#include <process.h>
#include "matrix/Matrix.h"

typedef struct {
    unsigned long **res;
    unsigned long **mat1;
    unsigned long **mat2;
    unsigned long to_line;
    unsigned long from_line;
    unsigned long mat1_cols;
    unsigned long mat2_cols;
} Data;

int counter = 0;

void multi_thread_multiplication(void *n_data) {
    Data *data = (Data *) n_data;
    for (unsigned long i = data->from_line; i < data->to_line; i++)
        for (unsigned long j = 0; j < data->mat2_cols; j++)
            for (unsigned long k = 0; k < data->mat1_cols; k++)
                data->res[i][j] += data->mat1[i][k] * data->mat2[k][j];
    counter++;
}

#endif //PARALLEL_HELPER_H