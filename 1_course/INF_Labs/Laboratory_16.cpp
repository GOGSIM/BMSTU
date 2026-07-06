/*
Задание: с клавиатуры вводится символьная строка. Используя рекурсивный алгоритм, 
определить количество символов #,$,@,*,&,% в заданной символьной строке. 
Реализовать программу с использованием двух видов подпрограмм – процедуры и функции. 
Вывести на экран всю необходимую информацию с соответствующими комментариями.
*/

#include <stdio.h>
#include <string.h>

int is_spec(char c){
    return c == '#' || c == '$' || c == '@' || c == '*' || c == '&' || c == '%';
}

int count_func(const char s[]){
    if (*s == '\0')
        return 0;
    return is_spec(*s) + count_func(s + 1);
}

void count_proc(const char s[], int& k){
    if (*s == '\0')
        return;
    k += is_spec(*s);
    count_proc(s + 1, k);
}

int main(){
    char s[256];
    int len;
    int k_proc = 0;
    int k_func;

    printf("Program counts symbols: #, $, @, *, &, %% in a string.\n");
    printf("Enter string: ");

    if (fgets(s, sizeof(s), stdin) == NULL){
        printf("Input error.\n");
        return 1;
    }

    len = (int)strlen(s);

    if (len > 0 && s[len - 1] == '\n'){
        s[len - 1] = '\0';
        len--;
    }

    if (len == 0){
        printf("Empty input.\n");
        return 1;
    }

    printf("Input string: %s\n", s);

    k_func = count_func(s);
    count_proc(s, k_proc);

    printf("Result using function: %d\n", k_func);
    printf("Result using procedure: %d\n", k_proc);

    return 0;
}