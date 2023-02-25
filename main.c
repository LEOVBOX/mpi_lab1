#include <stdio.h>
#include <mpi.h>
#include "matrix.h"
#include "mm_malloc.h"
#include "nonlinear_equation.h"

#define ROOT_THREAD 0
#define N_TAG 11
#define MATRIX_TAG 10
#define VECTORX_TAG 12
#define VECTORB_TAG 14
#define RESULT_TAG 13

int main(int argc, char* argv[])
{
	int size = 0;
	int rank = 0;
	MPI_Status status;

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

	// Корневой процесс
	if (rank == ROOT_THREAD)
	{
		double startTime = MPI_Wtime();
		//printf("Введите n - размер матрицы\n");
		// Размер матрицы
		int n = 2000;

		//printf("Should be %d iterations\n", n/size);

		/*if (scanf("%d", &n) == EOF)
		{
			printf("scanf error\n");
		}*/

		// Инициализация вектора
		double* vectorB = (double*)malloc(sizeof(double) * n);
		initVectorB(vectorB, &n);
		double* vectorX = (double*)malloc(sizeof(double) * n);
		initVectorX(vectorX, &n);

		//printf("Вектор на который умножать\n");
		//printArray(vector, &n);
		//printf("\n");

		// Инициализация матрицы
		double** matrix = (double**)malloc(sizeof(double*) * n);
		for (int i = 0; i < n; i++)
		{
			matrix[i] = (double*)malloc(sizeof(double) * n);
		}
		initMatrix(matrix, &n);
		printf("Matrix initialized\n");
		//printMatrix(matrix, &n);

		double* result = (double*)malloc(sizeof(double) * n);

		mpiMultMatrixByVectorRoot(result, matrix, vectorX, vectorB, &n, &size, &status);

		//printArray(result, &n);

		//printArray(result, &n);
		free(result);
		free(vectorX);
		free(vectorB);
		for (int i = 0; i < n; i++)
			free(matrix[i]);
		free(matrix);

		printf("Matrix was deleted\n");

		double endTime = MPI_Wtime();
		printf("TIME: %lf\n", endTime - startTime);
	}

	// Не корневой процесс
	else
	{
		int n = 0;
		MPI_Recv(&n, 1, MPI_INT, ROOT_THREAD, N_TAG, MPI_COMM_WORLD, &status);
		//printf("Process %d: n received\n", rank);

		double *vectorB = (double*)malloc(sizeof(double) * n);
		MPI_Recv(vectorB, n, MPI_DOUBLE, ROOT_THREAD, VECTORB_TAG, MPI_COMM_WORLD, &status);
		//printf("Process %d: vectorB received\n", rank);

		mpiMultVectorByVector(&rank, &size, vectorB, &n, &status);

	}

	// Завершение работы MPI
	MPI_Finalize();

	return 0;
}
