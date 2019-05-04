#include "matrix/Matrix.h"
#include "helper.h"

int main()
{
    Matrix mat1(N, M), mat2(M, K), res(N, K);

    res.zeros();
    mat1.random_values();
    mat2.random_values();

    static Data data[THREAD_COUNT];
    unsigned long from_line = 0;
    unsigned long to_line;
    unsigned long q = mat1.get_rows() / THREAD_COUNT;
    unsigned long r = mat1.get_rows() % THREAD_COUNT;

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        data[i].from_line = from_line;
        to_line = from_line + q + (i < r ? 1 : 0);
        data[i].to_line = to_line;
        from_line = to_line;
        data[i].mat1 = mat1.get_mat();
        data[i].mat2 = mat2.get_mat();
        data[i].res = res.get_mat();
        data[i].mat1_cols = mat1.get_cols();
        data[i].mat2_cols = mat2.get_cols();
    }

    auto&& start = std::chrono::steady_clock::now();

    for (int i = 0; i < THREAD_COUNT - 1; i++)
        _beginthread(multi_thread_multiplication, 0, (void*)&data[i]);

    multi_thread_multiplication((void*)&data[THREAD_COUNT - 1]);

    while (counter != THREAD_COUNT);

    auto&& end = std::chrono::steady_clock::now();
    auto&& diff = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "Time taken: " << diff << " ms" << std::endl;
    return 0;
}