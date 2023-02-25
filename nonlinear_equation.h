//
// Created by Леонид Шайхутдинов on 25.02.2023.
//

#ifndef MPI_LAB1_NONLINEAR_EQUATION_H
#define MPI_LAB1_NONLINEAR_EQUATION_H
#include <stdio.h>

// Инициализирует матрицу с главной диагональю равной 2. Остальные значения равны 1.
void initMatrix(double **matrix, int const *n);
void initVectorX(double *vector, int const *n);
void initVectorB(double *vector, int const *n);
void sendRows(double **matrix, int const *n, int const *size);
void recvResults(double *result, int const *n, int const *size, MPI_Status *status);
void mpiMultMatrixByVectorRoot(double *resBuffer, double **matrix, double *vectorX, double *vectorB,
		int const *n, int const *size, MPI_Status *status);
void recvMultSend(int const *rank, double *row, double *vector, double const *b, int const *n, MPI_Status *status);
void mpiMultVectorByVector(int const *rank, int const *size, double *vectorB, int const *n, MPI_Status *status);
#endif //MPI_LAB1_NONLINEAR_EQUATION_H