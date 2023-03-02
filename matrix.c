//
// Created by Леонид Шайхутдинов on 16.02.2023.
//

#include <stdio.h>
#include "mm_malloc.h"


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

void printArray(double *array, int const *n)
{
	for (int i = 0; i < *n; i++)
	{
		printf("%f ", array[i]);
	}
	printf("\n");
}

void printMatrix(double **matrix, int const *n)
{
	for (int i = 0; i < *n; i++)
	{
		printArray(matrix[i], n);
	}
}

double multVectors(double *row, double *column, int const *n)
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

double* vectorSub(double *vector1, double const *vector2, int const *n)
{
	double* result = vector1;
	for (int i = 0; i < *n; i++)
	{
		result[i] = vector1[i] - vector2[i];
	}
	return result;
}

double* multVectorByNum(double *vector, double const *num, int const *n)
{
	double* result = vector;
	for (int i = 0; i < *n; i++)
	{
		result[i] = vector[i] * *num;
	}
	return result;
}

double **mallocMatrix(int const *n)
{
	double** matrix = (double**)malloc(sizeof(double*) * (*n));
	for (int i = 0; i < *n; i++)
	{
		matrix[i] = (double*)malloc(sizeof(double) * (*n));
	}
	return matrix;
}



//double* paralleMultlMatrixByVector()