//1 ex

#include <stdio.h>
#include <math.h>

int main() {
    float y, ap, op;

    y = 1; 
    printf("Before transformations float y=%20.16f\n", y);

    y = y / 3;
    printf("Representation of number 1/3 in memory (variable) y=%20.16f\n", y);
    printf("Exact representation of number 1/3 (literal)  =%20.16f\n", 1.0 / 3.0);

    ap = fabs(1.0 / 3 - y); // Абсолютная погрешность
    op = ap / fabs(1.0 / 3); // Относительная погрешность
    printf("Absolute error of 1/3 representation ap=%20.16f\n", ap);
    printf("Relative error of 1/3 representation op=%20.16f\n", op);

    // Дальнейшие вычисления
    y /= 6000;
    y = exp(y);
    y = sqrt(y);
    y /= 14;
    y *= 14;
    y *= y;
    y = log(y);
    y *= 6000;
    y *= 3;

    printf("After transformations y=%20.16f\n", y);

    ap = fabs(1 - y);  // Абсолютная погрешность
    op = ap / 1; // Относительная погрешность
    printf("Absolute error ap=%20.16f\n", ap);
    printf("Relative error op=%20.16f\n", op);

    return 0;
}

//2 ex
#include <stdio.h>
#include <math.h>
#include <cmath>


int main() {
	float y, y1, y2, op, ap;
	int x = 5;
	y1 = (exp(x) + exp(-x)) / 2;
	y2 = (exp(x) - exp(-x)) / 2;	
       y = y1*y1 - y2*y2;
	ap = abs(1 - y);
	op = ap / 1;
	printf("x = %d\n", x);
	printf("y1 = ch(x) = %20.16f\n", y1);
	printf("y2 = sh(x) = %20.16f\n", y2);
	printf("y = ch^2(x) - sh^2(x) = %20.16f\n", y);
	printf("ap = |1 - y| = %20.16f\n", ap);
	printf("op = ap/1 = %20.16f\n", op);
}
