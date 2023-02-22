//
// Created by Леонид Шайхутдинов on 16.02.2023.
//

#include <stdio.h>


void scanfArray(int *array, int *n)
{
	for (int i = 0; i < *n; i++)
	{
		scanf("%d", &array[i]);
	}
}

void scanfMatrix(int **matrix, int *n)
{
	for (int i = 0; i < *n; i++)
	{
		scanfArray(matrix[i], n);
	}
}

void printArray(int *array, int *n)
{
	for (int i = 0; i < *n; i++)
	{
		printf("%d ", array[i]);
	}
	printf("\n");
}

void printMatrix(int **matrix, int *n)
{
	for (int i = 0; i < *n; i++)
	{
		printArray(matrix[i], n);
	}
}

void initMatrix(int **matrix, int *n)
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

int multVectors(int *row, int *column, int *n)
{
	int result = 0;
	for (int i = 0; i < *n; i++)
	{
		result += (row[i] * column[i]);
	}
	return result;
}

int* multMatrixByVector(int **matrix, int *vector, int *result, int *n)
{
	for (int i = 0; i < *n; i++)
	{
		result[i] = multVectors(matrix[i], vector, n);
	}
}