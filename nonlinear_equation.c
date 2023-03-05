//
// Created by Леонид Шайхутдинов on 25.02.2023.
//
#include "mpi.h"
#include <stdio.h>
#include "matrix.h"
#include "math.h"
#include "constants.h"

void initDefaultMatrix(double** matrix, int const* n)
{
	for (int i = 0; i < *n; i++)
	{
		for (int j = 0; j < *n; j++)
		{
			if (i == j)
			{
				matrix[i][j] = 2;
			}
			else
				matrix[i][j] = 1;
		}
	}
}

void initVectorB(double* vector, int const* n)
{
	for (int i = 0; i < *n; i++)
	{
		vector[i] = *n + 1;
	}
}

void initVectorX(double* vector, int const* n)
{
	for (int i = 0; i < *n; i++)
	{
		vector[i] = 0.0;
	}
}

void sendCrit(int const* crit, int const *threadCount)
{
	for (int toThread = 1; toThread < *threadCount; toThread++)
	{
		MPI_Send(crit, 1, MPI_INT, toThread, CRITERION_TAG, MPI_COMM_WORLD);
	}
}

void sendForChildren(void* buf, int count, int threadCount, int tag)
{
	for (int to_thread = 1; to_thread < threadCount; to_thread++)
	{
		MPI_Send(buf, count, MPI_DOUBLE, to_thread, tag, MPI_COMM_WORLD);
	}
}

void sendRows(double** matrix, int const* n, int const* threadCount)
{
	int toThread = 0;
	for (int row = 0; row < *n; row++)
	{
		if (toThread == *threadCount)
			toThread = 0;
		if (toThread != 0)
		{
			MPI_Send(matrix[row], *n, MPI_DOUBLE, toThread, MATRIX_TAG, MPI_COMM_WORLD);
			//printf("row %d sent to process %d\n ", row, toThread);
		}
		toThread++;
	}
}

void recvRows(double** subMatrix, int const* subMatrixSize, int const* n, MPI_Status *status)
{
	for (int i = 0; i < *subMatrixSize; i++)
	{
		MPI_Recv(subMatrix[i], *n, MPI_DOUBLE, ROOT_THREAD, MATRIX_TAG, MPI_COMM_WORLD, status);
		//printf("recvRows i = %d\n", i);
	}
}

void recvResults(double* resBuffer, int const* n, int const* threadCount, MPI_Status* status)
{
	int fromThread = 0;
	for (int row = 0; row < *n; row++)
	{
		if (fromThread == *threadCount)
			fromThread = 0;
		if (fromThread != 0)
		{
			MPI_Recv(&resBuffer[row], *threadCount, MPI_DOUBLE, fromThread, RESULT_TAG, MPI_COMM_WORLD, status);
		}
		//printf("result %d recv\n", row);
		fromThread++;
	}
}



void calcIterationRoot(double* resBuffer, double** matrix, double* vectorX, const double* vectorB,
		int const* n, int const* threadCount, MPI_Status* status)
{
	if (*threadCount > 1)
	{
		sendForChildren(vectorX, *n, *threadCount, VECTORX_TAG);
		recvResults(resBuffer, n, threadCount, status);
		//printf("Root: recvResults done\n");
	}

	double resultNum;
	for (int row = 0; row < *n; row += *threadCount)
	{
		// Ax
		resultNum = multVectors(vectorX, matrix[row], n);
		// (Ax-b)
		resultNum = resultNum - vectorB[0];
		resBuffer[row] = resultNum;
	}

}

double euclideanNorm(double const* vector, int const* n)
{
	double result = 0;
	for (int i = 0; i < *n; i++)
	{
		result += vector[i] * vector[i];
	}
	result = sqrt(result);
	return result;
}

int calcCriterion(double const* denominator, double const* vectorB, int const* n)
{
	double vectorBNorm = euclideanNorm(vectorB, n);
	double denominatorNorm = euclideanNorm(denominator, n);
	double crit = (denominatorNorm / vectorBNorm);
	//printf("Root: crit = %f , epsilon = %f\n", crit, EPSILON);
	if (crit < EPSILON)
		return 1;
	return 0;
}

int isAddtionRow(int const* rank, int const* threadCount, int const* n)
{
	if ((*rank) + 1 <= ((*n) % (*threadCount)))
		return 1;
	return 0;
}

void calcIteration(int const* rank, double** subMatrix, int const* subMatrixSize, double* vectorX, double* vectorB, int const* n,
		MPI_Status* status)
{

	MPI_Recv(vectorX, *n, MPI_DOUBLE, ROOT_THREAD, VECTORX_TAG, MPI_COMM_WORLD, status);
	//printf("Process %d: vectorX recv", *rank);

	double resultNum = 0;
	for (int i = 0; i < *subMatrixSize; i++)
	{
		// Ax
		resultNum = multVectors(vectorX, subMatrix[i], n);
		// (Ax-b)
		resultNum = resultNum - vectorB[(*rank)];
		// Отправляем полученную координату нового вектора x в root.
		MPI_Send(&resultNum, 1, MPI_DOUBLE, ROOT_THREAD, RESULT_TAG, MPI_COMM_WORLD);
		//printf("Process %d: %d iteration done\n", *rank, i);
	}
}