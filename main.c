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
	int n = N_NUM;

	// Инициализация вектора
	double* vectorB = (double*)malloc(sizeof(double) * n);
	initVectorB(vectorB, &n);
	//printf("VectorB initialised \n");

	double* vectorX = (double*)malloc(sizeof(double) * n);
	initVectorX(vectorX, &n);

	// Корневой процесс
	if (rank == ROOT_THREAD)
	{
		double startTime = MPI_Wtime();

		// Инициализация матрицы
		double** matrix = mallocMatrix(&n);
		initDefaultMatrix(matrix, &n);
		printf("Matrix initialized\n");
		//printMatrix(matrix, &n);

		// Вектор для хранения результата (Ax - b)
		double* result = (double*)malloc(sizeof(double) * n);

		sendRows(matrix, &n, &threadCount);

		while (crit == 0)
		{
			calcIterationRoot(result, vectorX, &n, &threadCount, &status);
			crit = calcCriterion(result, vectorB, &n);
			sendCrit(&crit, &threadCount);

			for (int i = 0; i < n; ++i)
			{
				result[i] *= TAU;
				vectorX[i] -= result[i];
			}
		}
		printf("Root: calculation done\n");

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

		// Инициализация подматрицы
		int subMatrixSize = n / ((threadCount) - 1);
		int isAdditionRow = (rank <= (n % ((threadCount) - 1)));
		//printf("Process %d: isAdditionRow = %d\n", rank, isAdditionRow);
		if (isAdditionRow == 1)
		{
			subMatrixSize += 1;
		}

		double** subMatrix = (double**)malloc(sizeof(double*) * subMatrixSize);
		for (int i = 0; i < subMatrixSize; i++)
		{
			subMatrix[i] = (double*)malloc(sizeof(double) * n);
		}

		// Получение строк матрицы
		recvRows(subMatrix, &subMatrixSize, &n, &isAdditionRow, &status);


		while (crit == 0)
		{
			// Подсчет (Ax - b)
			calcIteration(&rank, subMatrix, &subMatrixSize, vectorX, vectorB, &n, &status);
			MPI_Recv(&crit, 1, MPI_INT, ROOT_THREAD, CRITERION_TAG, MPI_COMM_WORLD, &status);
			//printf("Process %d: recieved calcCriterion = %d\n", rank, crit);
		}
		printf("Process %d: Calculation done\n", rank);

		free(vectorX);
		free(vectorB);
		//printf("Process %d: vectorB was free\n", rank);
	}

	//if (MPI_Probe(ROOT_THREAD, MPI_ANY_TAG, MPI_COMM_WORLD, &status) == MPI_SUCCESS)
		//printf("Process %d: waiting for recv\n", rank);



	// Завершение работы MPI
	MPI_Finalize();
	return 0;
}
