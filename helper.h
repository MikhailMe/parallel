#pragma once

#ifndef PARALLEL_HELPER_H
#define PARALLEL_HELPER_H

#include "matrix/Matrix.h"

Matrix single_thread_multiplication(Matrix &fm, Matrix &sm)
{
    if (fm.get_cols()!= sm.get_rows())
        throw std::exception();

    Matrix result(fm.get_rows(), sm.get_cols());

    for(unsigned long i = 0; i < fm.get_rows(); i++)
    {
        for (unsigned long j = 0; j < sm.get_cols(); j++)
        {
            unsigned long temp = 0;
            for (auto&& k = 0; k < fm.get_cols(); k++)
            {
                temp += fm.get_mat()[i][k] * sm.get_mat()[k][j];
            }
            result.set_elem_by_index(i, j, temp);
        }
    }
    return result;
}

#endif //PARALLEL_HELPER_H
