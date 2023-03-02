#include <stdio.h>
#include <mpi.h>
#include "matrix.h"
#include "mm_malloc.h"
#include "nonlinear_equation.h"
#include "constants.h"

int main(int argc, char* argv[])
{
	int threadCount = 0;
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
	MPI_Comm_size(MPI_COMM_WORLD, &threadCount);
	// Получение номера процесса
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int crit = 0;
	// Корневой процесс

	if (rank == ROOT_THREAD)
	{
		double startTime = MPI_Wtime();
		//printf("Введите n - размер матрицы\n");
		// Размер матрицы
		int n = N_NUM;

		//printf("Should be %d iterations\n", n/size);

		/*if (scanf("%d", &n) == EOF)
		{
			printf("scanf error\n");
		}*/

		// Инициализация вектора
		double* vectorB = (double*)malloc(sizeof(double) * n);
		initVectorB(vectorB, &n);
		printf("VectorB initialised: \n");
		//printArray(vectorB , &n);
		double* vectorX = (double*)malloc(sizeof(double) * n);
		initVectorX(vectorX, &n);
		printf("VectorX initialised: \n");
		//printArray(vectorX, &n);

		//printf("Вектор на который умножать\n");
		//printArray(vector, &n);
		//printf("\n");

		// Инициализация матрицы
		double** matrix = mallocMatrix(&n);
		initDefaultMatrix(matrix, &n);
		printf("Matrix initialized\n");
		//printMatrix(matrix, &n);

		// Вектор для хранения результата (Ax - b)
		double* result = (double*)malloc(sizeof(double) * n);

		sendVectors(vectorX, vectorB, &n, &threadCount);

		while (crit == 0)
		{
			calcIterationRoot(result, matrix, vectorX, vectorB, &n, &threadCount, &status);
			crit = calcCriterion(result, vectorB, &n);
			sendCrit(&crit, &threadCount);
			for (int i = 0; i < n; ++i)
			{
				result[i] *= TAU;
				vectorX[i] -= result[i];
			}
			//printf("Root: crit was calculated = %d\n", crit);
		}
		printf("Root: calculation done\n");
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
		printf("Process %d: n received\n", rank);

		double* vectorB = (double*)malloc(sizeof(double) * n);
		MPI_Recv(vectorB, n, MPI_DOUBLE, ROOT_THREAD, VECTORB_TAG, MPI_COMM_WORLD, &status);
		printf("Process %d: vectorB received\n", rank);
		double* vectorX = (double*)malloc(sizeof(double) * n);

		while (crit == 0)
		{
			// Подсчет (Ax - b)
			calcIteration(&rank, &threadCount, vectorX, vectorB, &n, &status);
			MPI_Recv(&crit, 1, MPI_INT, ROOT_THREAD, CRITERION_TAG, MPI_COMM_WORLD, &status);
			printf("Process %d: recieved calcCriterion = %d\n", rank, crit);
		}
		printf("Process %d: Calculation done\n", rank);

		free(vectorX);
		free(vectorB);
		printf("Process %d: vectorB was free\n", rank);
	}
	//MPI_Barrier(MPI_COMM_WORLD);
	if (MPI_Probe(ROOT_THREAD, MPI_ANY_TAG, MPI_COMM_WORLD, &status) == MPI_SUCCESS)
		printf("Process %d: waiting for \n", rank);



	// Завершение работы MPI
	MPI_Finalize();
	return 0;
}
