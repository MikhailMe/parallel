#pragma once

#ifndef PARALLEL_MATRIX_H
#define PARALLEL_MATRIX_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

class Matrix {

public:
    Matrix();
    Matrix(const Matrix&);
    Matrix(unsigned long n_rows, unsigned long n_cols);
    ~Matrix();

    unsigned long get_rows();
    unsigned long get_cols();
    unsigned long** get_mat();
    void set_elem_by_index(unsigned long n_i, unsigned long n_j, unsigned long n_value);

    void zeros();
    void read_full();
    void read_sizes();
    void read_values();
    void random_values();
    void print(std::string matrix);
    void read_sizes(unsigned long n_rows, unsigned long n_cols);

private:
    unsigned long rows;
    unsigned long cols;
    unsigned long** m;
    void allocate();
    void re_allocate();
};

#endif //PARALLEL_MATRIX_H
