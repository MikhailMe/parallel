#pragma once

#ifndef PARALLEL_HELPER_H
#define PARALLEL_HELPER_H

#include <chrono>
#include <process.h>
#include "matrix/Matrix.h"

#define THREAD_COUNT 4

typedef struct
{
    unsigned long from_line;
    unsigned long to_line;
} Data;

int counter = 0;
static Matrix mat1, mat2, res;

void multi_thread_multiplication(void *n_data)
{
    Data *data = (Data*) n_data;

    for (unsigned long i = data->from_line; i < data->to_line; i++)
    {
        for (unsigned long j = 0; j < mat2.get_cols(); j++)
        {
            unsigned long temp = 0;
            for (unsigned long k = 0; k < mat1.get_cols(); k++)
            {
                temp += mat1.get_mat()[i][k] * mat2.get_mat()[k][j];
            }
            res.set_elem_by_index(i, j, temp);
        }
    }
    counter++;
}

#endif //PARALLEL_HELPER_H
