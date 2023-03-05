//
// Created by Леонид Шайхутдинов on 25.02.2023.
//

#ifndef MPI_LAB1_NONLINEAR_EQUATION_H
#define MPI_LAB1_NONLINEAR_EQUATION_H

#include <stdio.h>

// Инициализирует матрицу с главной диагональю равной 2. Остальные значения равны 1.
void initDefaultMatrix(double **matrix, int const *n);
void initVectorX(double *vector, int const *n);
void initVectorB(double *vector, int const *n);
void sendRows(double **matrix, int const *n, int const *size);
void recvResults(double *result, int const *n, int const *size, MPI_Status *status);
void calcIterationRoot(double* resBuffer, double* vectorX, int const* n, int const* threadCount, MPI_Status* status);
void calcIteration(int const* rank, double** subMatrix, int const* subMatrixSize, double* vectorX, double* vectorB, int const* n,
		MPI_Status* status);
int calcCriterion(double const* denominator, double const* vectorB, int const* n);
double euclideanNorm(double const* vector, int const* n);
void sendCrit(int const* crit, int const *threadCount);
void sendForChildren(void* buf, int count, int threadCount, int tag);
void recvRows(double** subMatrix, int const* subMatrixSize, int const* n, int const* isAdditionRow,
		MPI_Status *status);
#endif //MPI_LAB1_NONLINEAR_EQUATION_H