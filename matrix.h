//
// Created by Леонид Шайхутдинов on 22.02.2023.
//

#ifndef MPI_LAB1_MATRIX_H
#define MPI_LAB1_MATRIX_H

void scanfArray(int *array, int *n);
void scanfMatrix(int **matrix, int *n);
void printArray(int *array, int *n);
void printMatrix(int **matrix, int *n);
void initMatrix(int **matrix, int *n);
int multVectors(int *row, int *column, int *n);
int* multMatrixByVector(int **matrix, int *vector, int *result, int *n);


#endif //MPI_LAB1_MATRIX_H
