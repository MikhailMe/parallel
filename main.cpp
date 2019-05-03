#include "matrix/Matrix.h"
#include "helper.h"

int main()
{
    Matrix mat1, mat2;

    mat1.read_sizes(500, 500);
    mat2.read_sizes(500, 500);

    mat1.random_values();
    mat2.random_values();

    try
    {
        auto&& start = std::chrono::steady_clock::now();
        auto&& mat = single_thread_multiplication(mat1, mat2);
        auto&& end = std::chrono::steady_clock::now();
        auto&& diff = std::chrono::duration<double, std::milli> (end-start).count();
        std::cout << "Time taken: " << diff << " ms" << std::endl;
    } catch (std::exception& e)
    {
        std::cout << "Illegal matrices sizes" << std::endl;
    }

    return 0;
}