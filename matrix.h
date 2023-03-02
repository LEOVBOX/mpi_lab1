//
// Created by Леонид Шайхутдинов on 22.02.2023.
//

#ifndef MPI_LAB1_MATRIX_H
#define MPI_LAB1_MATRIX_H

void scanfArray(double *array, int *n);
void scanfMatrix(double **matrix, int *n);
void printArray(double *array, int const *n);
void printMatrix(double **matrix, int const *n);
double multVectors(double *row, double *column, int const *n);
double* multMatrixByVector(double **matrix, double *vector, double *result, int *n);
double* vectorSub(double *vector1, double const *vector2, int const *n);
double** mallocMatrix(int const *n);

#endif //MPI_LAB1_MATRIX_H
