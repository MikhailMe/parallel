#include "Matrix.h"

Matrix::Matrix() : rows(1), cols(1)
{
    Matrix::allocate();
    Matrix::zeros();
}

Matrix::Matrix(unsigned long n_rows, unsigned long n_cols) : rows(n_rows), cols(n_cols)
{
    Matrix::allocate();
    Matrix::zeros();
}

Matrix::Matrix(const Matrix& mat) : rows(mat.rows), cols(mat.cols)
{
    Matrix::allocate();
    for (unsigned long i = 0; i < rows; i++)
        for (unsigned long j = 0; j < cols; j++)
            m[i][j] = mat.m[i][j];
}

Matrix::~Matrix()
{
    for(unsigned long i = 0; i < rows; i++)
        delete[] m[i];
    delete[] m;
}

void Matrix::zeros()
{
    for (unsigned long i = 0; i < rows; i++)
        for (unsigned long j = 0; j < cols; j++)
            m[i][j] = 0;
}

void Matrix::allocate()
{
    m = new unsigned long*[rows];
    for (unsigned long i = 0; i < rows; i++)
        m[i] = new unsigned long[cols];
}

void Matrix::re_allocate()
{
    m = new unsigned long*[rows];
    for (unsigned long i = 0; i < rows; i++)
        m[i] = new unsigned long[cols];
}

void Matrix::read_sizes()
{
    std::cout << "please enter matrix size: [rows][cols]" << std::endl;
    try
    {
        std::cin >> rows;
        std::cin >> cols;

        if (rows <= 0 || cols <= 0)
            throw std::exception();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << ": value below zero" << std::endl;
    }
    Matrix::re_allocate();
}

void Matrix::read_sizes(unsigned long n_rows, unsigned long n_cols)
{
    rows = n_rows;
    cols = n_cols;

    try
    {
        if (rows <= 0 || cols <= 0)
            throw std::exception();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << ": value below zero" << std::endl;
    }
    Matrix::re_allocate();
}

void Matrix::read_values()
{
    std::cout << "please enter matrix[" << rows << "][" << cols << "]" << std::endl;
    std::cout << "your enter will line by line" << std::endl;
    for (unsigned long i = 0; i < rows; i++)
        for (unsigned long j = 0; j < cols; j++)
            std::cin >> m[i][j];
}

void Matrix::read_full()
{
    Matrix::read_sizes();
    Matrix::re_allocate();
    Matrix::read_values();
}

void Matrix::print(std::string matrix)
{
    std::cout << "Matrix " << matrix << ":" << std::endl;
    for (unsigned long i = 0; i < rows; i++)
    {
        for (unsigned long j = 0; j < cols; j++)
            std::cout << m[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Matrix::random_values()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    for (unsigned long i = 0; i < rows; i++)
        for (unsigned long j = 0; j < cols; j++)
            m[i][j] = static_cast<unsigned long>(rand() % 100);
}

unsigned long Matrix::get_rows()
{
    return rows;
}

unsigned long Matrix::get_cols()
{
    return cols;
}

unsigned long **Matrix::get_mat()
{
    return m;
}

void Matrix::set_elem_by_index(unsigned long n_i, unsigned long n_j, unsigned long n_value)
{
    m[n_i][n_j] = n_value;
}