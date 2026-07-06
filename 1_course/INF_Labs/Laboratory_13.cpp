/*
Лабораторная работа 13. Использование указателей при работе со строками С++. Динамические строки. Текстовая обработка.
С клавиатуры вводится текст (последовательность строк), содержащий более 10 слов различной длины. 
Заданный текст распечатать по строкам, понимая под строкой либо последовательность из 12 символов, 
если в нее не входит пробел, либо часть текста до пробела. Пользуясь указателями выдать адреса букв третьего слова.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINEBUF 512

int countWords(const char *text) {
    const char *p = text;
    int words = 0;
    int inWord = 0;

    while (*p != '\0') {
        if (isspace((unsigned char)*p)) {
            inWord = 0;
        } else {
            if (!inWord) {
                words++;
                inWord = 1;
            }
        }
        p++;
    }
    return words;
}

void appendText(char **text, int *len, int *cap, const char *src) {
    int add = (int)strlen(src);

    while ((*len + add + 1) > *cap) {
        *cap = (*cap) * 2;
    }

    *text = (char*)realloc(*text, *cap);

    memcpy((*text) + (*len), src, add);
    *len += add;
    (*text)[*len] = '\0';
}

void printByRule(const char *text) {
    const char *p = text;

    printf("\nPrinting text in lines (rule 12 characters / before a space):\n");

    while (*p != '\0') {
        while (*p != '\0' && isspace((unsigned char)*p)) {
            p++;
        }

        if (*p != '\0'){
            int k = 0;
            int hasSpace = 0;
            const char* q = p;

            while (*q != '\0' && k < 12){
                if (*q == ' '){
                    hasSpace = 1;
                }
                q++;
                k++;
            }

            if (hasSpace){
                while (*p != '\0' && *p != ' '){
                    putchar(*p);
                    p++;
                }
                putchar('\n');
            } else {
                int i = 0;
                while (*p != '\0' && i < 12){
                    putchar(*p);
                    p++;
                    i++;
                }
                putchar('\n');
            }
        }
    }
}

void printThirdWordAddresses(char *text){
    char *p = text;
    int wordNum = 0;
    int found = 0;

    while (*p != '\0' && !found){
        while (*p != '\0' && isspace((unsigned char)*p)) p++;

        if (*p != '\0'){
            wordNum++;
            char *start = p;

            while (*p != '\0' && !isspace((unsigned char)*p)) p++;
            char *end = p;

            if (wordNum == 3){
                found = 1;

                printf("Third word: ");
                char *t1 = start;
                while (t1 < end){
                    putchar(*t1);
                    t1++;
                }
                printf("Letters's adresses: ");

                char *t2 = start;
                while (t2 < end){
                    printf("'%c' -> %p\n", *t2, (void*)t2);
                    t2++;
                }
            }
        }
    }
    if (!found){
        printf("Third word not found");
    }
}

int main() {
    int cap = 256;
    int len = 0;
    char *text = (char*)malloc(cap);
    text[0] = '\0';

    printf("Enter text (multiple lines).\n");
    printf("To finish input, enter an empty line.\n");

    char line[LINEBUF];

    int done = 0;
    while (!done) {
        if (fgets(line, sizeof(line), stdin) != NULL) {
            if (strcmp(line, "\n") == 0) {
                done = 1;
            } else {
                appendText(&text, &len, &cap, line);
            }

        } else {
            done = 1;
        }
    }

    printf("\nOriginal text:\n%s\n", text);

    int w = countWords(text);
    printf("Word count: %d\n", w);

    printByRule(text);
    printThirdWordAddresses(text);

    free(text);
    return 0;
}
