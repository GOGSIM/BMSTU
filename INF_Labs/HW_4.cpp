/*
Задача: выполнить структурную декомпозицию задачи. 
Разработать модуль, содержащий указанные процедуры и функции. 
Написать тестирующую программу.
Из заданной матрицы размером N*M (M>N) удалить столбцы, 
номера которых записаны в одномерном массиве из D целочисленных элементов в произвольном порядке. 
Вывести всю необходимую информацию с комментариями.
*/

//MatrixFunc.h
#ifndef MATRIXFUNC_H
#define MATRIXFUNC_H

int** createMatrix(int n, int m);

void printMatrix(int** matr, int n, int m, const char* message);

void deleteColumns(int**& matr, int& n, int& m, int* cols, int d);

#endif

//MatrixFunc.cpp
//#include "MatrixFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int** createMatrix(int n, int m) {
    int** matr = new int*[n];
    for (int i = 0; i < n; i++) {
        matr[i] = new int[m];
        for (int j = 0; j < m; j++) {
            *(*(matr + i) + j) = rand() % 199 - 99;
        }
    }
    return matr;
}

void printMatrix(int** matr, int n, int m, const char* message) {
    printf("\n%s\n", message);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%4d ", *(*(matr + i) + j));
        }
        printf("\n");
    }
}

void deleteColumns(int**& matr, int& n, int& m, int* cols, int d) {
    if (d == 0) {
        printf("No coloms for delete.\n");
        return;
    }

    int* deleted = new int[m]();  

    for (int k = 0; k < d; k++) {
        int col = *(cols + k);
        if (col >= 0 && col < m) {
            *(deleted + col) = 1;
        } 
    }

    int newM = 0;
    for (int j = 0; j < m; j++) {
        if (*(deleted + j) == 0) newM++;
    }

    if (newM == 0) {
        for (int i = 0; i < n; i++) {
            delete[] matr[i];
        }
        delete[] matr;
        matr = nullptr;
        m = 0;
        delete[] deleted;
        printf("All coloms was delete.\n");
        return;
    }

    int** newMatr = new int*[n];
    for (int i = 0; i < n; i++) {
        newMatr[i] = new int[newM];
        int newJ = 0;
        for (int j = 0; j < m; j++) {
            if (*(deleted + j) == 0) {
                *(*(newMatr + i) + newJ) = *(*(matr + i) + j);
                newJ++;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        delete[] matr[i];
    }
    delete[] matr;
    delete[] deleted;

    matr = newMatr;
    m = newM;
}

//main.cpp
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include "MatrixFunc.h"

//cd BMSTU/IU6
//g++ main.cpp MatrixFunc.cpp -o main
//./main

void sortArray(int* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (*(arr + j) > *(arr + j + 1)) {
                int temp = *(arr + j);
                *(arr + j) = *(arr + j + 1);
                *(arr + j + 1) = temp;
            }
        }
    }
}

int removeDuplicates(int* arr, int size) {
    if (size <= 1) return size;
    int newSize = 1;
    for (int i = 1; i < size; i++) {
        if (*(arr + i) != *(arr + newSize - 1)) {
            *(arr + newSize) = *(arr + i);
            newSize++;
        }
    }
    return newSize;
}

int main() {
    srand((unsigned)time(NULL));

    int N, M, D;

    printf("========================================\n");
    printf("   Deleting columns by numbers\n");
    printf("========================================\n");

    printf("Enter the number of rows N: ");
    scanf("%d", &N);
    do {
        printf("Enter the number of columns M (M > N): ");
        scanf("%d", &M);
        if (M <= N) {
            printf("Error: M must be greater than N. Please enter again.\n");
        }
    } while (M <= N);

    int** matrix = createMatrix(N, M);
    printMatrix(matrix, N, M, "The original matrix:");

    printf("\nEnter the number of columns to delete D: ");
    scanf("%d", &D);

    if (D <= 0) {
        printf("D <= 0, no deletion required.\n");
        printMatrix(matrix, N, M, "The matrix is unchanged:");

        for (int i = 0; i < N; i++) delete[] matrix[i];
        delete[] matrix;
        return 0;
    }

    int* cols = new int[D];
    printf("Enter %d column numbers (from 0 to %d) in any order:\n", D, M - 1);
    for (int i = 0; i < D; i++) {
        printf("  colomn %d: ", i + 1);
        scanf("%d", cols + i);
    }

    printf("\n--- The original array of numbers ---\n");
    for (int i = 0; i < D; i++) {
        printf("%d ", *(cols + i));
    }
    printf("\n");

    sortArray(cols, D);
    
    int newD = removeDuplicates(cols, D);
    
    deleteColumns(matrix, N, M, cols, newD);

    if (M == 0) {
        printf("\nThere are no columns left in the matrix.\n");
    } else {
        printMatrix(matrix, N, M, "\nThe matrix after deleting the columns:");
    }

    delete[] cols;
    if (matrix != nullptr) {
        for (int i = 0; i < N; i++) {
            delete[] matrix[i];
        }
        delete[] matrix;
    }

    printf("\nThe program is completed.\n");
    return 0;
}


/*
Задача: выполнить структурную декомпозицию, разработать структурную схему, 
содержащую не менее 3 подпрограмм, и алгоритмы этих подпрограмм. 
Реализовать на С++ в консольном режиме. 
Предусмотреть примитивный интерфейс типа меню, позволяющий выбирать нужную подпрограмму.
Разработать программу, которая реализует операции над векторами. 
Реализовать следующие операции: поэлементный ввод координат векторов, 
сумму векторов, разность векторов, произведение вектора на скаляр, 
проверку равенства векторов, вывод результатов операции на экран. 
 Обеспечить вывод всей необходимой информации в удобном и понятном виде. 
*/

//vector_ops.h
#ifndef VECTOR_OPS_H
#define VECTOR_OPS_H

void inputVector(double* vec, int n, const char* name);

void printVector(const double* vec, int n, const char* name);

void addVectors(const double* a, const double* b, double* res, int n);

void subVectors(const double* a, const double* b, double* res, int n);

void mulVectorByScalar(const double* vec, double scalar, double* res, int n);

int areEqual(const double* a, const double* b, int n);

#endif

//vector_ops.cpp
//#include "vector_ops.h"
#include <stdio.h>

void inputVector(double* vec, int n, const char* name) {
    printf("Enter the coordinates of vector %s (%d elements):\n", name, n);
    for (int i = 0; i < n; i++) {
        printf("  %s[%d] = ", name, i);
        scanf("%lf", vec + i);  
    }
}

void printVector(const double* vec, int n, const char* name) {
    printf("%s = (", name);
    for (int i = 0; i < n; i++) {
        printf("%.2f", *(vec + i));
        if (i < n - 1) printf(", ");
    }
    printf(")\n");
}

void addVectors(const double* a, const double* b, double* res, int n) {
    for (int i = 0; i < n; i++) {
        *(res + i) = *(a + i) + *(b + i);
    }
}

void subVectors(const double* a, const double* b, double* res, int n) {
    for (int i = 0; i < n; i++) {
        *(res + i) = *(a + i) - *(b + i);
    }
}

void mulVectorByScalar(const double* vec, double scalar, double* res, int n) {
    for (int i = 0; i < n; i++) {
        *(res + i) = *(vec + i) * scalar;
    }
}

int areEqual(const double* a, const double* b, int n) {
    for (int i = 0; i < n; i++) {
        if (*(a + i) != *(b + i)) return 0;
    }
    return 1;
}

//main.cpp
#include <stdio.h>
#include <stdlib.h>
//#include "vector_ops.h"

//cd BMSTU/IU6
//g++ main.cpp vector_ops.cpp -o main
//./main

int main() {
    int n;

    printf("========================================\n");
    printf("           Vector operations\n");
    printf("========================================\n");
    printf("Enter the dimension of vectors: ");
    scanf("%d", &n);

    if (n <= 0) {
        printf("Error: dimension must be positive.\n");
        return 1;
    }

    double* v1 = (double*)malloc(n * sizeof(double));
    double* v2 = (double*)malloc(n * sizeof(double));
    double* vres = (double*)malloc(n * sizeof(double));

    if (!v1 || !v2 || !vres) {
        printf("Memory allocation error.\n");
        free(v1); free(v2); free(vres);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        *(v1 + i) = 0.0;
        *(v2 + i) = 0.0;
    }

    int choice;
    int v1_defined = 0, v2_defined = 0;  

    do {
        printf("\n--- Menu ---\n");
        printf("1. Input vector V1\n");
        printf("2. Input vector V2\n");
        printf("3. Output vectors\n");
        printf("4. Sum V1 + V2\n");
        printf("5. Difference V1 - V2\n");
        printf("6. Multiply vector by scalar\n");
        printf("7. Check equality of V1 and V2\n");
        printf("0. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                inputVector(v1, n, "V1");
                v1_defined = 1;
                break;
            case 2:
                inputVector(v2, n, "V2");
                v2_defined = 1;
                break;
            case 3:
                if (v1_defined) printVector(v1, n, "V1");
                else printf("V1 is not defined.\n");
                if (v2_defined) printVector(v2, n, "V2");
                else printf("V2 is not defined.\n");
                break;
            case 4:
                if (!v1_defined || !v2_defined) {
                    printf("Error: both vectors must be defined.\n");
                } else {
                    addVectors(v1, v2, vres, n);
                    printf("Result of addition:\n");
                    printVector(v1, n, "V1");
                    printVector(v2, n, "V2");
                    printVector(vres, n, "V1 + V2");
                }
                break;
            case 5:
                if (!v1_defined || !v2_defined) {
                    printf("Error: both vectors must be defined.\n");
                } else {
                    subVectors(v1, v2, vres, n);
                    printf("Result of subtraction:\n");
                    printVector(v1, n, "V1");
                    printVector(v2, n, "V2");
                    printVector(vres, n, "V1 - V2");
                }
                break;
            case 6:
                {
                    int vecChoice;
                    double scalar;
                    printf("Choose vector (1 - V1, 2 - V2): ");
                    scanf("%d", &vecChoice);
                    if (vecChoice == 1 && !v1_defined) {
                        printf("Error: V1 is not defined.\n");
                        break;
                    }
                    if (vecChoice == 2 && !v2_defined) {
                        printf("Error: V2 is not defined.\n");
                        break;
                    }
                    printf("Enter scalar: ");
                    scanf("%lf", &scalar);
                    if (vecChoice == 1) {
                        mulVectorByScalar(v1, scalar, vres, n);
                        printf("Result of multiplying V1 by %.2f:\n", scalar);
                        printVector(v1, n, "V1");
                        printVector(vres, n, "V1 * scalar");
                    } else {
                        mulVectorByScalar(v2, scalar, vres, n);
                        printf("Result of multiplying V2 by %.2f:\n", scalar);
                        printVector(v2, n, "V2");
                        printVector(vres, n, "V2 * scalar");
                    }
                }
                break;
            case 7:
                if (!v1_defined || !v2_defined) {
                    printf("Error: both vectors must be defined.\n");
                } else {
                    if (areEqual(v1, v2, n)) {
                        printf("Vectors are equal.\n");
                    } else {
                        printf("Vectors are not equal.\n");
                    }
                }
                break;
            case 0:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    free(v1);
    free(v2);
    free(vres);
    return 0;
}
