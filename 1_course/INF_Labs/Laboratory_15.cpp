/*
а) Текстовые файлы
Сформировать текстовый файл символьных строк, состоящих из слов, разделенных пробелом, программным путем. 
В сформированном файле определить номер строки, содержащей максимальное количество слов. 
Удалить из файла эту строку. Вывести на экран, сформированный и полученный файлы, 
а также номер найденной строки и найденное количество слов в этой строке.

б) Двоичные файлы
Используя датчик случайных чисел сформировать файл целых чисел в диапазоне от 0 до 99. 
Переписать их в другой файл в обратном порядке, одновременно выкидывая четные числа. 
Вывести на экран исходный и полученный файлы.
*/


//Part A
#include <stdio.h>
#include <string.h>

const int MAX_LEN = 256;

void createFile(const char* f_name) {
    FILE* f = fopen(f_name, "w");
    if (f == NULL) {
        printf("Error: failed to create file %s\n", f_name);
        return;
    }

    fputs("alpha betta gamma delita epsilon zelta\n", f);
    fputs("one two\n", f);
    fputs("cat dog\n", f);
    fputs("sun moon star\n", f);
    fputs("alloha\n", f);

    fclose(f);
    printf("File %s successfully created.\n", f_name);
}

int fileExists(const char* f_name) {
    FILE* f = fopen(f_name, "r");
    if (f == NULL) {
        return 0;
    }
    fclose(f);
    return 1;
}

int countWords(char* line) {
    char buffer[MAX_LEN];
    strcpy(buffer, line);

    int count = 0;
    char* token = strtok(buffer, " \t\n");

    while (token != NULL) {
        count++;
        token = strtok(NULL, " \t\n");
    }

    return count;
}

void printFile(const char* f_name, const char* title) {
    FILE* f = fopen(f_name, "r");
    if (f == NULL) {
        printf("Error: file %s does not exist or cannot be opened.\n", f_name);
        return;
    }

    char line[MAX_LEN];

    printf("\n%s:\n", title);
    printf("--------------------------------\n");

    while (fgets(line, MAX_LEN, f) != NULL) {
        printf("%s", line);
    }

    printf("--------------------------------\n");

    fclose(f);
}

void findMaxWordsLine(const char* f_name, int* max_line_n, int* maxWords) {
    FILE* f = fopen(f_name, "r");
    if (f == NULL) {
        printf("Error: file %s does not exist or cannot be opened.\n", f_name);
        *max_line_n = -1;
        *maxWords = 0;
        return;
    }

    char line[MAX_LEN];
    int curLine = 0;
    *max_line_n = 0;
    *maxWords = -1;

    while (fgets(line, MAX_LEN, f) != NULL) {
        curLine++;
        int words = countWords(line);

        if (words > *maxWords) {
            *maxWords = words;
            *max_line_n = curLine;
        }
    }

    fclose(f);
}

void removeLineFromFile(const char* f_name, int del_line) {
    FILE* in = fopen(f_name, "r");
    FILE* out = fopen("temp.txt", "w");

    if (in == NULL || out == NULL) {
        printf("Error opening files.\n");

        if (in != NULL) fclose(in);
        if (out != NULL) fclose(out);
        return;
    }

    char line[MAX_LEN];
    int curLine = 0;

    while (fgets(line, MAX_LEN, in) != NULL) {
        curLine++;
        if (curLine != del_line) {
            fputs(line, out);
        }
    }

    fclose(in);
    fclose(out);

    remove(f_name);
    rename("temp.txt", f_name);

    printf("Line number %d has been deleted.\n", del_line);
}

void showMenu() {
    printf("\n============= MENU =============\n");
    printf("1. Create file\n");
    printf("2. Show file content\n");
    printf("3. Find line with maximum number of words\n");
    printf("4. Delete line with maximum number of words\n");
    printf("5. Run full task\n");
    printf("0. Exit\n");
    printf("================================\n");
    printf("Enter menu option: ");
}

void clearInputBuffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

int main() {
    char f_name[] = "text.txt";
    int choice;
    int max_line_n = -1;
    int maxWords = 0;
    int lineFound = 0;

    do {
        showMenu();

        if (scanf("%d", &choice) != 1) {
            printf("Error: enter an integer from 0 to 5.\n");
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();

        switch (choice) {
            case 1:
                createFile(f_name);
                break;

            case 2:
                if (!fileExists(f_name)) {
                    printf("File %s does not exist. Create it first.\n", f_name);
                } else {
                    printFile(f_name, "File content");
                }
                break;

            case 3:
                if (!fileExists(f_name)) {
                    printf("File %s does not exist. Create it first.\n", f_name);
                } else {
                    findMaxWordsLine(f_name, &max_line_n, &maxWords);

                    if (max_line_n <= 0) {
                        printf("Failed to find the required line.\n");
                    } else {
                        printf("Line with maximum words: %d\n", max_line_n);
                        printf("Number of words: %d\n", maxWords);
                        lineFound = 1;
                    }
                }
                break;

            case 4:
                if (!fileExists(f_name)) {
                    printf("File %s does not exist. Create it first.\n", f_name);
                } else {
                    if (!lineFound) {
                        findMaxWordsLine(f_name, &max_line_n, &maxWords);
                    }

                    if (max_line_n <= 0) {
                        printf("No line to delete.\n");
                    } else {
                        removeLineFromFile(f_name, max_line_n);
                        printFile(f_name, "File after deletion");
                        lineFound = 0;
                    }
                }
                break;

            case 5:
                createFile(f_name);
                printFile(f_name, "Original file");

                findMaxWordsLine(f_name, &max_line_n, &maxWords);

                if (max_line_n <= 0) {
                    printf("Error processing file.\n");
                } else {
                    printf("Line with maximum words: %d\n", max_line_n);
                    printf("Number of words: %d\n", maxWords);

                    removeLineFromFile(f_name, max_line_n);
                    printFile(f_name, "Result file");
                }
                lineFound = 0;
                break;

            case 0:
                printf("Program terminated.\n");
                break;

            default:
                printf("Error: invalid menu option.\n");
        }

    } while (choice != 0);

    return 0;
}


//Part B
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void create_random_file(const char* f_name, int count){
    FILE* f = fopen(f_name, "wb");
    if (f == NULL){
        printf("Error: cannot create file %s\n", f_name);
        return;
    }

    for (int i = 0; i < count; i++){
        int n = rand() % 100;   
        fwrite(&n, sizeof(int), 1, f);
    }

    fclose(f);
}

void create_res_file(const char* inputF, const char* outputF){
    FILE* in = fopen(inputF, "rb");
    FILE* out = fopen(outputF, "wb");

    if (in == NULL || out == NULL){
        printf("Error: cannot open files.\n");

        if (in != NULL) fclose(in);
        if (out != NULL) fclose(out);

        return;
    }

    fseek(in, 0, SEEK_END);
    long size = ftell(in);
    int count = (int)(size / sizeof(int));

    for (int i = count - 1; i >= 0; i--){
        fseek(in, i * sizeof(int), SEEK_SET);

        int n;
        fread(&n, sizeof(int), 1, in);

        if (n % 2 != 0)  {
            fwrite(&n, sizeof(int), 1, out);
        }
    }

    fclose(in);
    fclose(out);
}

void print_bin_file(const char* f_name, const char* title){
    FILE* f = fopen(f_name, "rb");
    if (f == NULL){
        printf("Error: cannot open file %s\n", f_name);
        return;
    }

    int n;
    int index = 1;

    printf("\n%s:\n", title);
    printf("----------------------------------------\n");

    while (fread(&n, sizeof(int), 1, f) == 1){
        printf("%4d) %4d\n", index, n);
        index++;
    }

    if (index == 1){
        printf("File is empty.\n");
    }

    printf("----------------------------------------\n");

    fclose(f);
}

int main(){
    const char inputF[] = "input.bin";
    const char outputF[] = "output.bin";
    int count;

    printf("The program creates a binary file of random integers from 0 to 99,\n");
    printf("rewrites them into another file in reverse order,\n");
    printf("and removes even numbers.\n\n");

    printf("Enter number of elements: ");
    while (scanf("%d", &count) != 1 || count <= 0){
        printf("Incorrect input. Enter an integer > 0: ");
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
    }

    srand((unsigned)time(NULL));

    create_random_file(inputF, count);
    print_bin_file(inputF, "Initial binary file");

    create_res_file(inputF, outputF);
    print_bin_file(outputF, "Result binary file");

    return 0;
}
