/*
РК1 семестр 2. Вариант 18.
Выполнить структурную декомпозицию программы на несколько подпрограмм. 
В основной программе ввести размер массива и вывести результаты работы выделенных подпрограмм.
Пользователь вводит строку, состоящую из слов, разделенных точками без пробелов 
(размер строки определяется при вводе, как длина введенной строки и под нее выделяется память). 
Вывести исходную строку. Найти в строке все слова, 
в которых заданная буква встречается более двух раз подряд (буква задается пользователем). 
Найденные слова распечатать в порядке, обратном алфавитному. Если нужных слов нет, то выдать сообщение. 
Вывести всю необходимую информацию.
*/

#include <stdio.h>
#include <string.h>

void free_str(int i, int count_w, char** words, char* text) {
    i = 0;
    while (i < count_w) {
        delete[] *(words + i);
        i++;
    }

    delete[] words;
    delete[] text;
}

void solution(char* text, int i, char letter, int& count_w, char** words) {
    while (*(text + i) != '\0') {
        int start = i;

        while (*(text + i) != ' ' && *(text + i) != '\0') {
            i++;
        }

        int len = i - start;

        if (len > 0) {
            int f = 0;
            int j = 0;

            while (j < len) {
                int count = 0;

                while (j + count < len && *(text + start + j + count) == letter) {
                    count++;
                }

                if (count > 2) {
                    f = 1;
                }

                if (count > 0) {
                    j += count;
                } else {
                    j++;
                }
            }

            if (f == 1) {
                *(words + count_w) = new char[len + 1];

                int k = 0;
                while (k < len) {
                    *(*(words + count_w) + k) = *(text + start + k);
                    k++;
                }

                *(*(words + count_w) + len) = '\0';
                count_w++;
            }
        }

        if (*(text + i) == ' ') {
            i++;
        }
    }
}

int main() {
    char* text;
    char letter;
    int n, i;

    printf("Input size of string: ");
    scanf_s("%d", &n);
    getchar();

    text = new char[n + 1];

    printf("Input string: ");
    fgets(text, n + 1, stdin);

    if (*(text + strlen(text) - 1) == '\n') {
        *(text + strlen(text) - 1) = '\0';
    }

    printf("\nEnter letter: ");
    scanf_s(" %c", &letter, 1);

    char** words = new char*[n];
    int count_w = 0;

    i = 0;
    solution(text, i, letter, count_w, words);

    if (count_w == 0) {
        printf("No words in string!");
    } else {
        int pass = 0;

        while (pass < count_w - 1) {
            int pos = 0;

            while (pos < count_w - pass - 1) {
                if (strcmp(*(words + pos), *(words + pos + 1)) < 0) {
                    char* temp = *(words + pos);
                    *(words + pos) = *(words + pos + 1);
                    *(words + pos + 1) = temp;
                }
                pos++;
            }
            pass++;
        }

        printf("\nFound words in string:\n");
        i = 0;
        while (i < count_w) {
            printf("%s\n", *(words + i));
            i++;
        }
    }

    free_str(i, count_w, words, text);

    getchar();
    getchar();
    return 0;
}