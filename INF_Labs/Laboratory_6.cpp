/*
решить задачу, используя процедуру или функцию. Выбор обосновать. На примере полученной программы продемонстрировать умение:
1)	назначать точку останова;
2)	выполнить программу по шагам с заходом в процедуры и без захода;
3)	определять значения переменных на конкретном шаге.
Дана матрица целых чисел A(n, m) (n, m <= 10). Числа находятся в диапазоне от -10 до 10. 
Найдите минимальный элемент в каждой строке и в зависимости от его знака выполните сдвиг строки влево или вправо 
на количество элементов равное модулю этого минимального элемента. 
Выведите начальную матрицу, с дополнительным столбцом справа с минимальными элементами и полученную в результате сдвигов матрицу.
Для решения данной задачи понадобится 6 функций. В таблице 1 приведены функции и их назначения
*/

#include <stdio.h>

#define MAX 10

void read_matrix(int a[][MAX], int n, int m) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            scanf("%d", &a[i][j]);
}

int row_min(const int *row, int m) {
    int mn = row[0];
    for (int j = 1; j < m; ++j)
        if (row[j] < mn) mn = row[j];
    return mn;
}

void shift_left(int *row, int m, int k) {
    if (m == 0) return;
    k %= m; if (k == 0) return;
    int tmp[MAX];
    for (int j = 0; j < m; ++j)
        tmp[j] = row[(j + k) % m];
    for (int j = 0; j < m; ++j)
        row[j] = tmp[j];
}

void shift_right(int *row, int m, int k) {
    if (m == 0) return;
    k %= m; if (k == 0) return;
    shift_left(row, m, m - k);
}

void print_matrix_with_mins(const int a[][MAX], int n, int m, const int mins[]) {
    printf("Initial matrix with row minima on the right:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j)
            printf("%4d", a[i][j]);
        printf("  |%4d\n", mins[i]);
    }
    printf("\n");
}

void print_matrix(const int a[][MAX], int n, int m, const char *title) {
    printf("%s\n", title);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j)
            printf("%4d", a[i][j]);
        printf("\n");
    }
    printf("\n");
}

int main(void) {
    int n, m;
    int A[MAX][MAX];

    printf("Enter n and m (both <= 10): ");
    scanf("%d %d", &n, &m);

    printf("Enter the %d x %d matrix elements (each in [-10..10]):\n", n, m);
    read_matrix(A, n, m);

    int B[MAX][MAX];
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            B[i][j] = A[i][j];

    int mins[MAX];
    for (int i = 0; i < n; ++i) {
        mins[i] = row_min(B[i], m);
    }
    print_matrix_with_mins((const int (*)[MAX])B, n, m, mins);
    for (int i = 0; i < n; ++i) {
        int mn = mins[i];
        int k = mn < 0 ? -mn : mn;  
        if (mn < 0)      shift_left(A[i], m, k);
        else if (mn > 0) shift_right(A[i], m, k);
    }

    print_matrix((const int (*)[MAX])A, n, m, "Result matrix after row shifts:");
    printf("Legend:\n");
    printf("  If row minimum < 0 -> cyclic left shift by |min| positions.\n");
    printf("  If row minimum > 0 -> cyclic right shift by min positions.\n");
    printf("  If row minimum = 0 -> no shift.\n");
    return 0;
}
