/*
Часть 1. Создание модулей. Указатель на функцию
Задание:
Разработать модуль, содержащий указанную процедуру. Написать тестирующую программу.
Составить подпрограмму-процедуру INTG вычисленияплощади, ограниченной заданной функцией и осью x на заданном отрезке по формуле: ...

В основной программе, являющейся тестирующей, проверить процедуру INTG на примере функций для вычисления площади функций: ...
Значение n  вводится с клавиатуры. Проверить программу при n=10,20,30. Результаты сравнить с точным значением, полученным в любом онлайн калькуляторе.

Часть 2. Строки и динамические структуры
Задание: дана символьная строка, состоящая из слов, разделенных пробелом. Составить программу, которая вводит строку, организует из ее слов однонаправленный список. 
Определить количество слов в списке, длина которых меньше 4 букв. Вывести на экран сформированный список и результат поиска. 
Если искомых слов нет – выдать соответствующее сообщение.
*/    

//part 1
// main programm
#include <stdio.h>
//#include "main.h"   

int main() {
    int n;

    printf("Numerical integration using rectangles method\n");
    printf("Formula: S = (b - a) / n * sum_{i=1..n} f(x_i)\n\n");

    printf("Enter n (number of partitions): ");
    scanf("%d", &n);

    printf("\nYou entered n = %d\n\n", n);

    double a1 = 0.1, b1 = 1.0;
    //double s1 = INTG(f1, a1, b1, n);

    printf("Function 1: f(x) = sin(x) / x\n");
    printf("Interval: [%.2f; %.2f]\n", a1, b1);
    //printf("Approximate area S1 = %.10f\n\n", s1);

    double a2 = 1.0, b2 = 2.0;
    //double s2 = INTG(f2, a2, b2, n);

    printf("Function 2: f(x) = tg(x+1) / (x+1)\n");
    printf("Interval: [%.2f; %.2f]\n", a2, b2);
    //printf("Approximate area S2 = %.10f\n\n", s2);

    printf("Compare these results with an online calculator.\n");

    return 0;
}
// realisation file
#ifndef MAIN_H
#define MAIN_H

#include <math.h>

typedef double (*FuncPtr)(double);

double INTG(FuncPtr f, double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.0;

    for (int i = 1; i <= n; i++) {
        double x = a + i * h;
        sum += f(x);
    }

    return h * sum;
}

double f1(double x) {
    return sin(x) / x;
}

double f2(double x) {
    return tan(x + 1.0) / (x + 1.0);
}

#endif

//part 2
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Node {
    char word[100];
    Node* next;
};

void addNode(Node*& head, const char* w) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->word, w);
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
        return;
    }

    Node* temp = head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = newNode;
}

void printList(Node* head) {
    Node* temp = head;
    printf("Linked list:\n");
    while (temp != NULL) {
        printf("%s\n", temp->word);
        temp = temp->next;
    }
}

int countShortWords(Node* head) {
    int count = 0;
    Node* temp = head;
    while (temp != NULL) {
        if (strlen(temp->word) < 4)
            count++;
        temp = temp->next;
    }
    return count;
}

int main() {
    char input[300];
    char* token;
    Node* head = NULL;

    printf("Enter a string: ");
    fgets(input, sizeof(input), stdin);

    token = strtok(input, " \n");

    while (token != NULL) {
        addNode(head, token);
        token = strtok(NULL, " \n");
    }

    if (head == NULL) {
        printf("The list is empty. No words found.\n");
        return 0;
    }

    printList(head);

    int result = countShortWords(head);

    if (result == 0) {
        printf("\nNo words shorter than 4 letters were found.\n");
    } else {
        printf("\nNumber of words shorter than 4 letters: %d\n", result);
    }

    return 0;
}
