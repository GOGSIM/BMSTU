/*
Дана прямая y = kx + b, k и b вводятся с клавиатуры, k не равно нулю. 
Дальше идут координаты 20 точек по OX на этой прямой. 
Найдите такую любую пару точек, что расстояние между ними минимально. 
Выведите координаты (x, y) точек в порядке ввода, затем номера искомой пары и их координаты.
*/

#include <stdio.h>
#include <math.h>

int main() {
    const int N = 20;
    double k, b;
    printf("Enter k and b: ");
    scanf("%lf %lf", &k, &b);

    double x[N], y[N];
    printf("Enter %d coordinates along the OX axis:\n", N);
    for (int i = 0; i < N; ++i) {
        scanf("%lf", &x[i]);
        y[i] = k * x[i] + b;
    }

    printf("\nCoordinates of points on the line:\n");
    for (int i = 0; i < N; ++i) {
        printf("%d: (%.2f, %.2f)\n", i + 1, x[i], y[i]);
    }

    int bi = 0, bj = 1;
    double best = 1e300; 

    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            double dx = x[j] - x[i];
            double dy = y[j] - y[i];
            double d2 = dx * dx + dy * dy; 
            if (d2 < best) {
                best = d2;
                bi = i;
                bj = j;
            }
        }
    }

    printf("\nClosest pair of points:\n");
    printf("Indexes: %d %d\n", bi + 1, bj + 1);
    printf("Coordinates: (%.2f, %.2f) (%.2f, %.2f)\n", x[bi], y[bi], x[bj], y[bj]);

    return 0;
}
