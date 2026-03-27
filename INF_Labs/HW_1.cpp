/*
Часть 1 Вычисление выражений
Вычислить уравнение
Определить, как влияет на точность вычислений выбранный тип данных.

Часть 2 Программирование разветвляющегося вычислительного процесса
Заданы три действительных числа x, y, z Присвоить логической переменной значение
true если только одно из этих чисел положительно. 
В противном случае логической переменной присвоить значение false
Протестировать все ветви алгоритма.
 */   

//part 1
#include <stdio.h>
#include <math.h>

int main(void) {
    double x;  
    printf("Enter x (radians): ");
    if (scanf("%lf", &x) != 1) {
        printf("Input error.\n");
        return 1;
    }

    double y_double = fabs((1.0 - fabs(sin(x))) / (4.0 * (1.0 + fabs(cos(x)))));

    float xf = (float)x;
    float y_float = fabsf((1.0f - fabsf(sinf(xf))) / (4.0f * (1.0f + fabsf(cosf(xf)))));

    int y_int = (int)y_double;

    printf("y (int)    = %d\n", y_int);
    printf("y (float)  = %.16f\n", (double)y_float); 
    printf("y (double) = %.16f\n", y_double);

    return 0;
}

//part 2
#include <stdio.h>

int main(void) {
    double x, y, z;
    bool result; 

    printf("Enter three real numbers (x y z): ");
    if (scanf("%lf %lf %lf", &x, &y, &z) != 3) {
        printf("Input error.\n");
        return 1;
    }

    int positive_count = 0;
    if (x > 0) positive_count++;
    if (y > 0) positive_count++;
    if (z > 0) positive_count++;

    result = (positive_count == 1);

    if (result)
        printf("Logical variable = true\n");
    else
        printf("Logical variable = false\n");

    return 0;
}



//part 3
#include <stdio.h>
#include <math.h>

int main(void) {
    const double x = 0.5;

    const double eps[]   = {1e-2, 1e-3, 1e-4};
    const char*  label[] = {"10^-2", "10^-3", "10^-4"};
    const int T = (int)(sizeof(eps)/sizeof(eps[0]));

    printf("Calculation of arctan(x) using a series at x = %.1f\n", x);
    printf("--------------------------------------------------------------------------------------\n");
    printf("%8s | %22s | %22s | %10s | %22s\n",
           "xi", "sum (series)", "arctan(x)", "iterations", "abs. error");
    printf("--------------------------------------------------------------------------------------\n");

    for (int t = 0; t < T; ++t) {
        double xi = eps[t];

        double sum  = 0.0;
        double term = x;     
        int k = 0;         

        while (fabs(term) >= xi) {
            sum += term;
            ++k;
            term *= -(x*x) * ((2.0*k - 1.0) / (2.0*k + 1.0));
        }

        double exact   = atan(x);
        double abs_err = fabs(sum - exact);

        printf("%8s | %22.16f | %22.16f | %10d | %22.16f\n",
               label[t], sum, exact, k, abs_err);
    }

    return 0;
}
