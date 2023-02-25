//
// Created by Леонид Шайхутдинов on 25.02.2023.
//
#include "mpi.h"
#include <stdio.h>
#include "matrix.h"
#include "mm_malloc.h"

#define ROOT_THREAD 0
#define N_TAG 11
#define MATRIX_TAG 10
#define VECTORX_TAG 12
#define VECTORB_TAG 14
#define RESULT_TAG 13


void initMatrix(double **matrix, int const *n)
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

void initVectorB(double *vector, int const *n)
{
	for (int i = 0; i < *n; i++)
	{
		vector[i] = *n+1;
	}
}

void initVectorX(double *vector, int const *n)
{
	for (int i = 0; i < *n; i++)
	{
		vector[i] = 0;
	}
}

void sendVectors(double const *vectorB, double const *vectorX, int const *n, int const *size)
{
	for (int to_thread = 1; to_thread < *size; to_thread++)
	{
		MPI_Send(n, 1, MPI_INT, to_thread, N_TAG, MPI_COMM_WORLD);
		MPI_Send(vectorX, *n, MPI_DOUBLE, to_thread, VECTORX_TAG, MPI_COMM_WORLD);
		MPI_Send(vectorB, *n, MPI_DOUBLE, to_thread, VECTORB_TAG, MPI_COMM_WORLD);
	}
}

void sendRows(double **matrix, int const *n, int const *size)
{

	int to_thread = 1;
	for (int row = 0; row < *n; row += 1)
	{
		if (to_thread == *size)
			to_thread = 1;
		//printf("to_thread = %d; size = %d; row = %d\n", to_thread, size, row);
		MPI_Send(matrix[row], *n, MPI_DOUBLE, to_thread, MATRIX_TAG, MPI_COMM_WORLD);
		//printf("row %d sent to process %d\n ", row, to_thread);
		to_thread++;
	}

}

void recvResults(double *resBuffer, int const *n, int const *size, MPI_Status *status)
{
	for (int i = 0; i < *n; i++)
	{
		MPI_Recv(&resBuffer[i], *size, MPI_DOUBLE, MPI_ANY_SOURCE, RESULT_TAG, MPI_COMM_WORLD, status);
		//printf("result %d row recv\n", i);
	}
}

void mpiMultMatrixByVectorRoot(double *resBuffer, double **matrix, double *vectorX, double *vectorB,
		int const *n, int const *size, MPI_Status *status)
{
	sendVectors(vectorB, vectorX, n, size);
	sendRows(matrix, n, size);
	recvResults(resBuffer, n, size, status);
}

void recvMultSend(int const *rank, double *row, double *vector, double *b, int const *n, MPI_Status *status)
{
	MPI_Recv(row, *n, MPI_DOUBLE, ROOT_THREAD, MATRIX_TAG, MPI_COMM_WORLD, status);
	//printf("Process %d: recv row: ", *rank);
	//printArray(row, n);

	double resultNum = multVectors(vector, row, n);
	resultNum -= *b;
	//printf("Process %d: multed, result = %f\n", *rank, resultNum);

	MPI_Send(&resultNum, 1, MPI_DOUBLE, ROOT_THREAD, RESULT_TAG, MPI_COMM_WORLD);
}

void mpiMultVectorByVector(int const *rank, int const *size, double *vectorB, int const *n, MPI_Status *status)
{

	double *vectorX = (double*)malloc(sizeof(double) * *n);
	MPI_Recv(vectorX, *n, MPI_DOUBLE, ROOT_THREAD, VECTORX_TAG, MPI_COMM_WORLD, status);

	double *row = (double*)malloc(sizeof(double) * *n);

	for (int i = 0; i < *n/((*size)-1); i++)
	{
		recvMultSend(rank, row, vectorX, &vectorB[*rank], n, status);
		//printf("Process %d: %d iteration done\n", *rank, i);
	}

	int isAdditionRow = (*rank <= (*n % ((*size) - 1)));
	//printf("Process %d: isAdditionRow = %d, addition = %d, size = %d\n", *rank, isAdditionRow, *n%((*size) - 1), *size);
	if (isAdditionRow)
	{
		recvMultSend(rank, row, vectorX, &vectorB[*rank], n, status);
		//printf("Process %d: addition iteration done\n", *rank);
	}

	//printf("Process %d: done\n", *rank);
}




