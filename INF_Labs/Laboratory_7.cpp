/*
Дана непустая последовательность слов из строчных латинских букв (количество слов больше 5): 
между соседними словами – запятая и пробел, за последним словом - точка. 
Определить количество слов, которые начинаются на гласную букву. 
Вывести на экран количество слов и сами эти слова.
*/    

#include <stdio.h>   
#include <string.h>  

int main() {
    char line[256];        
    char vowels[] = "aeiouy"; 

    printf("Enter the sequence of words: \n");
    fgets(line, 256, stdin);

    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
        len--;
    }

    if (len > 0 && line[len - 1] == '.') {
        line[len - 1] = '\0';
    }

    char goodWords[100][50]; 
    int goodCount = 0;      

    char *word = strtok(line, ", ");
    while (word != NULL) {
        char first = word[0];                 
        if (strchr(vowels, first) != NULL) { 
            strcpy(goodWords[goodCount], word);
            goodCount++;
        }

        word = strtok(NULL, ", ");
    }

    printf("The number of words that start with a vowel: %d\n", goodCount);
    if (goodCount > 0) {
        printf("These words:\n");
        for (int i = 0; i < goodCount; i++) {
            printf("%s\n", goodWords[i]);
        }
    }

    return 0;
}
