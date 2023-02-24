#include <stdio.h>
#include <mpi.h>
#include "matrix.h"
#include "mm_malloc.h"

#define ROOT_THREAD 0
#define N_TAG 11
#define MATRIX_TAG 10
#define VECTOR_TAG 12
#define COUNTER_TAG 14
#define RESULT_TAG 13
#define CANSEL_TAG 15

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
		printf("Введите n - размер матрицы\n");
		// Размер матрицы
		int n = 0;

		if (scanf("%d", &n) == EOF)
		{
			printf("scanf error\n");
		}

		// Инициализация вектора
		double* vector = (double*)malloc(sizeof(double) * n);
		for (int i = 0; i < n; i++)
		{
			vector[i] = i+1;
		}
		printf("Вектор на который умножать\n");
		printArray(vector, &n);
		printf("\n");

		// Инициализация матрицы
		double** matrix = (double**)malloc(sizeof(double*) * n);
		for (int i = 0; i < n; i++)
		{
			matrix[i] = (double*)malloc(sizeof(double) * n);
		}
		initMatrix(matrix, &n);
		//printf("Matrix initialized\n");
		printMatrix(matrix, &n);

		double* result = (double*)malloc(sizeof(double) * n);
		int *countRecv = (int*)malloc(sizeof(int));
		int to_thread = 1;

		// Рассылка всем процессам базовой инфы
		for (to_thread = 1; to_thread < size; to_thread++)
		{
			MPI_Send(&n, 1, MPI_INT, to_thread, N_TAG, MPI_COMM_WORLD);
			MPI_Send(vector, n, MPI_DOUBLE, to_thread, VECTOR_TAG, MPI_COMM_WORLD);
		}

		// Рассылка строк матрицы
		to_thread = 1;
		for (int row = 0; row < n; row += 1)
		{
			if (to_thread == size)
				to_thread = 1;
			//printf("to_thread = %d; size = %d; row = %d\n", to_thread, size, row);
			MPI_Send(matrix[row], n, MPI_DOUBLE, to_thread, MATRIX_TAG, MPI_COMM_WORLD);
			printf("row: %d sent to process %d\n ", row, to_thread);
			to_thread++;
			*countRecv += 1;
		}

		// Получение результатов перемножения строк матрицы на вектор
		for (int from_thread = 0; from_thread < n; from_thread++)
		{
			MPI_Recv(&result[from_thread], size, MPI_DOUBLE, MPI_ANY_SOURCE, RESULT_TAG, MPI_COMM_WORLD, &status);
			printf("result %d row recv\n", from_thread);
		}


		printArray(result, &n);


		free(result);
		free(vector);
		for (int i = 0; i < n; i++)
			free(matrix[i]);
		free(matrix);
		printf("Matrix was deleted\n");
	}

	// Не корневой процесс
	else
	{
		double resultNum = 0;
		int n = 0;
		int countQueue = 0;
		MPI_Recv(&n, 1, MPI_INT, ROOT_THREAD, N_TAG, MPI_COMM_WORLD, &status);
		//printf("Process %d: n = %d\n", rank, n);

		double *vector = (double*)malloc(sizeof(double) * n);
		MPI_Recv(vector, n, MPI_DOUBLE, ROOT_THREAD, VECTOR_TAG, MPI_COMM_WORLD, &status);


		double *row = (double*)malloc(sizeof(double) * n);

		for (int i = 0; i < n/size; i++)
		{
			MPI_Recv(row, n, MPI_DOUBLE, ROOT_THREAD, MATRIX_TAG, MPI_COMM_WORLD, &status);
			printf("Process %d: recv row: ", rank);
			printArray(row, &n);

			resultNum = multVectors(vector, row, &n);
			printf("Process %d: multed, result = %f\n", rank, resultNum);\

			MPI_Send(&resultNum, 1, MPI_DOUBLE, ROOT_THREAD, RESULT_TAG, MPI_COMM_WORLD);
		}

		if (rank <= n%size)
		{
			MPI_Recv(row, n, MPI_DOUBLE, ROOT_THREAD, MATRIX_TAG, MPI_COMM_WORLD, &status);
			printf("Process %d: recv row: ", rank);
			printArray(row, &n);

			resultNum = multVectors(vector, row, &n);
			printf("Process %d: multed, result = %f\n", rank, resultNum);\

			MPI_Send(&resultNum, 1, MPI_DOUBLE, ROOT_THREAD, RESULT_TAG, MPI_COMM_WORLD);
		}

		printf("Process %d: done\n", rank);

	}

	// Завершение работы MPI
	MPI_Finalize();
	return 0;
}
