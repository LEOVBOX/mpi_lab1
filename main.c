#include <stdio.h>
#include <mpi.h>
#include "matrix.h"
#include "mm_malloc.h"

#define FIRST_THREAD 0

int main(int argc, char* argv[])
{
	int size = 0;
	int rank = 0;
	// Инициализация MPI. Указываем сколько процессов должно создаться.
	int rc = MPI_Init(&argc, &argv);
	if (rc != MPI_SUCCESS)
	{
		printf("Mpi Initialisation error\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	// Получение числа инициализированных процессов.
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	// Получение номера процесса
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == FIRST_THREAD)
	{
		printf("Введите n - размер матрицы\n");
		// Размер матрицы
		int n = 0;
		if (scanf("%d", &n) == EOF)
		{
			printf("scanf error\n");
			MPI_Abort(MPI_COMM_WORLD, 0);
		}

		float **matrix = (float **)malloc(sizeof(float *) * n);
		for (int i = 0; i < n; i++)
		{
			matrix[i] = (float *)malloc(sizeof(float) * n);
		}

		float *vector = (float *)malloc(sizeof(float) * n);
		printf("Введите вектор на который умножать\n");
		scanfArray(vector, &n);

		initMatrix(matrix, &n);
		float *result = (float *)malloc(sizeof(float) * n);
		multMatrixByVector(matrix, vector, result, &n);
		/*for (int to_thread = 1; to_thread < size; to_thread++)
		{
			MPI_Send(matrix, n, MPI_INT, to_thread, 0, MPI_COMM_WORLD);
		}*/

		printArray(result, &n);
		free(result);
		free(vector);
		for (int i = 0; i < n; i++)
			free(matrix[i]);
		free(matrix);
	}


	MPI_Finalize(); // Завершение работы MPI
	return 0;
}
