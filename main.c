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
	//printf("threadCount = %d\n", threadCount);

	// Получение номера процесса
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int crit = 0;
	int n = N_NUM;

	// Инициализация вектора
	double* vectorB = (double*)malloc(sizeof(double) * n);
	initVectorB(vectorB, &n);
	//printf("VectorB initialised \n");

	double* vectorX = (double*)malloc(sizeof(double) * n);
	initVectorX(vectorX, &n);
	//printf("VectorX init\n");

	// Корневой процесс
	if (rank == ROOT_THREAD)
	{
		double startTime = MPI_Wtime();

		// Инициализация матрицы
		double** matrix = mallocMatrix(&n);
		initDefaultMatrix(matrix, &n);
		//printf("Matrix initialized\n");
		//printMatrix(matrix, &n);

		// Вектор для хранения результата (Ax - b)
		double* result = (double*)malloc(sizeof(double) * n);

		if (threadCount > 1)
		{
			sendRows(matrix, &n, &threadCount);
		}

		printf("sendRows passed\n");

		while (crit == 0)
		{
			calcIterationRoot(result, matrix, vectorX, vectorB, &n, &threadCount, &status);
			//printf("calcIterationRoot passed\n");
			crit = calcCriterion(result, vectorB, &n);
			if (threadCount > 1)
			{
				sendCrit(&crit, &threadCount);
			}


			for (int i = 0; i < n; ++i)
			{
				result[i] *= TAU;
				vectorX[i] -= result[i];
			}
		}
		printf("Root: calculation done\n");

		free(result);
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

		// Инициализация подматрицы.
		int subMatrixSize = n / threadCount;

		if (isAddtionRow(&rank, &threadCount, &n))
		{
			printf("Process %d: isAdditionRow = 1\n", rank);
			subMatrixSize += 1;
		}
		printf("Process %d: subMatrixSize = %d\n", rank, subMatrixSize);

		double** subMatrix = (double**)malloc(sizeof(double*) * subMatrixSize);
		for (int i = 0; i < subMatrixSize; i++)
		{
			subMatrix[i] = (double*)malloc(sizeof(double) * n);
		}

		// Получение строк матрицы
		recvRows(subMatrix, &subMatrixSize, &n, &status);
		printf("Process %d: rows recv\n", rank);
		while (crit == 0)
		{
			// Подсчет (Ax - b)
			calcIteration(&rank, subMatrix, &subMatrixSize, vectorX, vectorB, &n, &status);
			MPI_Recv(&crit, 1, MPI_INT, ROOT_THREAD, CRITERION_TAG, MPI_COMM_WORLD, &status);
			//printf("Process %d: recieved calcCriterion = %d\n", rank, crit);
		}
		for (int i = 0; i < subMatrixSize; i++)
		{
			free(subMatrix[i]);
		}
		free(subMatrix);

		printf("Process %d: Calculation done\n", rank);


		//printf("Process %d: vectorB was free\n", rank);
	}

	free(vectorX);
	free(vectorB);


	// Завершение работы MPI
	MPI_Finalize();
	return 0;
}
