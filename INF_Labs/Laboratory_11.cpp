//Лабораторная работа 11. Использование указателей при работе с массивами в С++. Динамические одномерные массивы. 
//Сформировать одномерный динамический целочисленный  массив А(n), (n вводится с клавиатуры).  
//Определить сумму  отрицательных  элементов массива и поместить ее на место последнего элемента этого массива. 
//Пользуясь указателями, вывести на экран адреса найденных отрицательных элементов. 
//Вывести на экран исходный и сформированный массивы, а также найденную сумму.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    int n;
    printf("Input size of massive: ");
    scanf("%d", &n);
    
    int *a = new int[n];
    srand(time(NULL));

    for (int i = 0; i < n; i++){
        *(a+i) = rand() % 101-50;
    }

    printf("\nFirst massive:\n");
    for (int i = 0; i<n; i++){
        printf("%d ", *(a+i));
    }
    printf("\n");

    int sum = 0;
    printf("\nAddresses of negative elements:\n");
    for (int i = 0; i < n; i++){
        if (*(a+i) < 0){
            sum += *(a+i);
            printf("%p\n", (void*)&a[i]);
        }
    }

    if (n > 0) {
        a[n - 1] = sum;
    }

    printf("\nSecond massive:\n");
    for (int i = 0; i<n; i++){
        printf("%d ", *(a+i));
    }

    printf("\nSum of negative elements: %d\n", sum);

    free(a);

    return 0;
}
