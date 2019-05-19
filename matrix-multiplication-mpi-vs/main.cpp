#include <chrono>
#include <process.h>
#include <mpi.h>
#include <conio.h>
#include <string>

#include "helper.h"
#include "matrix/matrix.h"

// NxM * MxK = NxK
#define N 1000
#define M 1000
#define K 1000
#define MASTER_TO_SLAVE_TAG 1
#define SLAVE_TO_MASTER_TAG 4

MPI_Status status; // MPI_Recv
MPI_Request request; // MPI_Isend

int main(int argc, char *argv[])
{
	int rank, proc_amount;
	double start_time, end_time;
	int portion, low_bound, upper_bound;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_amount);

	Matrix mat1(N, M), mat2(M, K), res(N, K);
	res.zeros();

	std::chrono::time_point<std::chrono::steady_clock> start;

	// мастер
	if (rank == 0) {
		
		mat1.random_values();
		mat2.random_values();

		start = std::chrono::steady_clock::now();
		// бежим по слейвам
		for (int i = 1; i < proc_amount; i++)
		{
			portion = (N / (proc_amount - 1));
			low_bound = (i - 1) * portion;
			if (((i + 1) == proc_amount) && ((N % (proc_amount - 1)) != 0))
				upper_bound = N; // последний slave получает все оставшиес€ строки
			else
				upper_bound = low_bound + portion; // строки mat1 дел€тс€ поровну между слевайми

			// отправл€ем нижнюю, верхнюю границы и выделенную часть строки mat1, не блокиру€, предполагаемому слейву
			MPI_Isend(&low_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &request);
			MPI_Isend(&upper_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &request);
			MPI_Isend(&mat1.get_mat()[low_bound][0], (upper_bound - low_bound) * N, MPI_DOUBLE, i, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &request);
		}
	}

	//посылаем mat2 всем слейвам
	MPI_Bcast(&*mat2.get_mat(), M*K, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	// —лейвы
	if (rank > 0) {

		// получаем нижнюю, верхнюю границы и строки mat1 от мастера
		MPI_Recv(&low_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&upper_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&mat1.get_mat()[low_bound][0], (upper_bound - low_bound) * M, MPI_DOUBLE, 0, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &status);
		
		for (int i = low_bound; i < upper_bound; i++)
		{
			for (int j = 0; j < K; j++)
			{
				int temp = 0;
				for (int k = 0; k < M; k++)
				{
					temp += (mat1.get_mat()[i][k] * mat2.get_mat()[k][j]);
				}
				res.set_elem_by_index(i, j, temp);
			}
		}
		// отправл€ем обратно нижнюю, верхнюю границы и часть результата без блокировки мастеру
		MPI_Isend(&low_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &request);
		MPI_Isend(&upper_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &request);
		MPI_Isend(&res.get_mat()[low_bound][0], (upper_bound - low_bound) * K, MPI_DOUBLE, 0, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &request);
	}

	// мастер собирает результаты
	if (rank == 0) 
	{
		for (int i = 1; i < proc_amount; i++)
		{
			// получаем нижнюю, верхнюю границы и результат от слейва
			MPI_Recv(&low_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &status);
			MPI_Recv(&upper_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &status);
			MPI_Recv(&res.get_mat()[low_bound][0], (upper_bound - low_bound) * K, MPI_DOUBLE, i, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &status);
		}
		auto&& end = std::chrono::steady_clock::now();
		auto&& diff = std::chrono::duration<double, std::milli>(end - start).count();
		std::cout << "Time taken: " << diff << " ms" << std::endl;
	}
	MPI_Finalize();
	_getch();
	return 0;
}