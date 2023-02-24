//
// Created by Леонид Шайхутдинов on 22.02.2023.
//

#ifndef MPI_LAB1_MATRIX_H
#define MPI_LAB1_MATRIX_H

void scanfArray(double *array, int *n);
void scanfMatrix(double **matrix, int *n);
void printArray(double *array, int *n);
void printMatrix(double **matrix, int *n);
void initMatrix(double **matrix, int *n);
double multVectors(double *row, double *column, int *n);
double* multMatrixByVector(double **matrix, double *vector, double *result, int *n);


#endif //MPI_LAB1_MATRIX_H
