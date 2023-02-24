//
// Created by Леонид Шайхутдинов on 16.02.2023.
//

#include <stdio.h>


void scanfArray(double *array, int *n)
{
	for (int i = 0; i < *n; i++)
	{
		scanf("%lf", &array[i]);
	}
}

void scanfMatrix(double **matrix, int *n)
{
	for (int i = 0; i < *n; i++)
	{
		scanfArray(matrix[i], n);
	}
}

void printArray(double *array, int *n)
{
	for (int i = 0; i < *n; i++)
	{
		printf("%f ", array[i]);
	}
	printf("\n");
}

void printMatrix(double **matrix, int *n)
{
	for (int i = 0; i < *n; i++)
	{
		printArray(matrix[i], n);
	}
}

void initMatrix(double **matrix, int *n)
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

double multVectors(double *row, double *column, int *n)
{
	double result = 0;
	for (int i = 0; i < *n; i++)
	{
		result += (row[i] * column[i]);
	}
	return result;
}

double* multMatrixByVector(double **matrix, double *vector, double *result, int *n)
{
	for (int i = 0; i < *n; i++)
	{
		result[i] = multVectors(matrix[i], vector, n);
	}
}