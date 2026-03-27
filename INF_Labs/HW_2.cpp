/*
Часть 1 Обработка одномерных массивов
Задание: сортировать одномерный массив вещественных чисел по не возрастанию. Использовать метод обменов.

Часть 2 Обработка матриц
Составить программу, которая в матрице A(n, m)
меняет местами строку, содержащую первый отрицательный элемент при просмотре по столбцам, 
и строку с максимальным положительным элементом всей матрицы. Вывести на печать исходную, 
полученную матрицы, а также всю промежуточную информацию (найденные элементы и их координаты). 
Если строки совпадают – выдать диагностическое сообщение.
*/

//part 1
#include <stdio.h>

int main() {
    int n;
    printf("Enter the number of elements: ");
    scanf("%d", &n);

    double arr[n];
    printf("Enter %d numbers separated by spaces: ", n);
    for (int i = 0; i < n; i++) {
        scanf("%lf", &arr[i]);
    }

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] < arr[j + 1]) {
                double temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }

        printf("After %d sort iteration: ", i + 1);
        for (int k = 0; k < n; k++) {
            printf("%.2lf ", arr[k]);
        }
        printf("\n");
    }

    return 0;
}

//part 2
#include <stdio.h>

#define MAXN 10
#define MAXM 10

void print_matrix(double a[MAXN][MAXM], int n, int m, const char* title) {
    printf("%s\n", title);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            printf("%8.2lf ", a[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    int n, m;
    printf("Enter n and m: ");
    scanf("%d %d", &n, &m);
    
    double A[MAXN][MAXM];

    printf("Enter the elements of the matrix %d x %d (row by row):\n", n, m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (scanf("%lf", &A[i][j]) != 1) {
                printf("Input error.\n");
                return 0;
            }
        }
    }

    print_matrix(A, n, m, "Initial matrix:");

    int row_first_neg = -1, r_neg = -1, c_neg = -1;
    double val_neg = 0.0;
    for (int j = 0; j < m && row_first_neg == -1; ++j) {
        for (int i = 0; i < n; ++i) {
            if (A[i][j] < 0.0) {
                row_first_neg = i;
                r_neg = i; c_neg = j; val_neg = A[i][j];
                break;
            }
        }
    }

    int row_max_pos = -1, r_pos = -1, c_pos = -1;
    double max_pos = -1e300; 
    int found_pos = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (A[i][j] > 0.0) {
                if (!found_pos || A[i][j] > max_pos) {
                    max_pos = A[i][j];
                    row_max_pos = i;
                    r_pos = i; c_pos = j;
                    found_pos = 1;
                }
            }
        }
    }

    if (row_first_neg != -1) {
        printf("First negative element (column-wise): %.2lf "
               "(row %d, column %d)\n",
               val_neg, r_neg + 1, c_neg + 1);
    } else {
        printf("No negative elements in the matrix.\n");
    }
    if (found_pos) {
        printf("Maximum positive element: %.2lf "
               "(row %d, column %d)\n",
               max_pos, r_pos + 1, c_pos + 1);
    } else {
        printf("No positive elements in the matrix.\n");
    }
    if (row_first_neg == -1 || !found_pos) {
        printf("Row swap is impossible (insufficient elements).\n");
        return 0;
    }
    if (row_first_neg == row_max_pos) {
        printf("Diagnostic message: rows coincide (row %d). Swap not required.\n",
               row_first_neg + 1);
        return 0;
    }
    for (int j = 0; j < m; ++j) {
        double tmp = A[row_first_neg][j];
        A[row_first_neg][j] = A[row_max_pos][j];
        A[row_max_pos][j] = tmp;
    }
    printf("Rows swapped: %d <-> %d completed.\n\n", row_first_neg + 1, row_max_pos + 1);

    print_matrix(A, n, m, "Matrix after row swap:");

    return 0;
}
