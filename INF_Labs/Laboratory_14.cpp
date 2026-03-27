/*
Задача: разработать модуль, содержащий указанные процедуры и функции. 
Написать тестирующую программу.
Сформировать квадратную матрицу порядка N (N вводится с клавиатуры). 
Дано число К (К <N, вводится с клавиатуры). Число K определяет диагональ, которая делит матрицу на две части. 
В каждой из показанных на рисунке 1 частей найти минимальный элемент и его номер. 
Вывести на экран исходную матрицу и найденные значения.
*/

//DiagParts.h
#ifndef DIAGPARTS_H
#define DIAGPARTS_H

struct MinInfo
{
    int value;
    int row;
    int col;
    int found;
};

int* createMatrix(int n);
void deleteMatrix(int *matrix);

void fillMatrixRandom(int *matrix, int n);
void printMatrix(const int *matrix, int n);

MinInfo findMinAboveDiagonal(const int *matrix, int n, int k);
MinInfo findMinBelowDiagonal(const int *matrix, int n, int k);

#endif

//DiagParts.cpp

//#include "DiagParts.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* createMatrix(int n)
{
    return new int[n * n];
}

void deleteMatrix(int *matrix)
{
    delete[] matrix;
}

void fillMatrixRandom(int *matrix, int n)
{
    srand(time(NULL));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            *(matrix + i * n + j) = rand() % 100 - 50;
        }
    }
}

void printMatrix(const int *matrix, int n)
{
    printf("\nInitial matrix:\n");

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%6d", *(matrix + i * n + j));
        }
        printf("\n");
    }
}

MinInfo findMinAboveDiagonal(const int *matrix, int n, int k)
{
    MinInfo result;
    result.found = 0;
    result.value = 0;
    result.row = -1;
    result.col = -1;

    int border = k - 1; 

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i + j < border)
            {
                int current = *(matrix + i * n + j);

                if (!result.found || current < result.value)
                {
                    result.value = current;
                    result.row = i;
                    result.col = j;
                    result.found = 1;
                }
            }
        }
    }

    return result;
}

MinInfo findMinBelowDiagonal(const int *matrix, int n, int k)
{
    MinInfo result;
    result.found = 0;
    result.value = 0;
    result.row = -1;
    result.col = -1;

    int border = k - 1;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i + j > border)
            {
                int current = *(matrix + i * n + j);

                if (!result.found || current < result.value)
                {
                    result.value = current;
                    result.row = i;
                    result.col = j;
                    result.found = 1;
                }
            }
        }
    }

    return result;
}

//main.cpp


#include <stdio.h>
//#include "DiagParts.h"

//cd BMSTU/IU6
//g++ main.cpp DiagParts.cpp -o main
//./main

int main()
{
    int n, k;

    printf("Enter matrix size N (N > 1): ");
    while (scanf("%d", &n) != 1 || n <= 1)
    {
        printf("Incorrect input. Enter an integer N > 1: ");
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
    }

    printf("Enter K (1 <= K < N): ");
    while (scanf("%d", &k) != 1 || k < 1 || k >= n)
    {
        printf("Incorrect input. Enter an integer K such that 1 <= K < N: ");
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
    }

    int *matrix = createMatrix(n);

    fillMatrixRandom(matrix, n);
    printf("\nGenerated matrix:\n");
    printMatrix(matrix, n);

    MinInfo part1 = findMinAboveDiagonal(matrix, n, k);
    MinInfo part2 = findMinBelowDiagonal(matrix, n, k);

    printf("\nDiagonal condition: i + j = %d (for 0-based indexes)\n", k - 1);
    printf("Part 1: i + j < %d\n", k - 1);
    printf("Part 2: i + j > %d\n", k - 1);

    if (part1.found)
    {
        printf("\nPart 1 minimum element: %d\n", part1.value);
        printf("Position in matrix: row = %d, column = %d\n", part1.row + 1, part1.col + 1);
        printf("Address of this element: %p\n", (void*)(matrix + part1.row * n + part1.col));
    }
    else
    {
        printf("\nPart 1 has no elements.\n");
    }

    if (part2.found)
    {
        printf("\nPart 2 minimum element: %d\n", part2.value);
        printf("Position in matrix: row = %d, column = %d\n", part2.row + 1, part2.col + 1);
        printf("Address of this element: %p\n", (void*)(matrix + part2.row * n + part2.col));
    }
    else
    {
        printf("\nPart 2 has no elements.\n");
    }

    deleteMatrix(matrix);
    return 0;
}