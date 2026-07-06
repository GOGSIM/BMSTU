/*
Дан массив целых чисел M размером 3*12. 
Массив сформирован с помощью генератора случайных чисел в диапазоне от -20 до +70. 
Определить среднее арифметическое отрицательных чисел матрицы. 
Вывести на экран исходную матрицу, количество отрицательных чисел и их среднее арифметическое.
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    const int ROWS = 3;
    const int COLS = 12;
    int M[ROWS][COLS];
    int i, j;
    int negativeCount = 0;
    int negativeSum = 0;

    srand(time(NULL));

    printf("Generated matrix (3x12):\n");

    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            M[i][j] = rand() % 91 - 20; 
            printf("%4d", M[i][j]);
            if (M[i][j] < 0) {
                negativeCount++;
                negativeSum += M[i][j];
            }
        }
        printf("\n");
    }

    double average = 0.0;
    if (negativeCount > 0) {
        average = (double)negativeSum / negativeCount;
    }

    printf("\nNumber of negative elements: %d\n", negativeCount);
    if (negativeCount > 0)
        printf("Average of negative elements: %.2f\n", average);
    else
        printf("No negative elements found.\n");

    return 0;
}
