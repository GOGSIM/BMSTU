/*
из текстового файла вводится символьная строка, состоящая из слов, 
разделенных пробелом. Составить программу, которая вводит строку, 
организует из ее слов однонаправленный список, отсортированный по алфавиту, 
считает в списке количество слов на гласную букву и количество слов на согласную, 
и определяет, каких слов в списке больше – на гласную или согласную. 
Вывести на экран созданный список и всю найденную информацию с соответствующими комментариями.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Node{
    char word[101];
    Node* next;
};

int isVowel(char ch){
    ch = (char)tolower((unsigned char)ch);

    if (ch == 'a' || ch == 'e' || ch == 'i' ||
        ch == 'o' || ch == 'u' || ch == 'y'){
        return 1;
    }

    return 0;
}

void insertSorted(Node** first, const char* newWord){
    Node* newNode = new Node;
    strcpy(newNode->word, newWord);
    newNode->next = NULL;

    if (*first == NULL){
        *first = newNode;
    }
    else{
        if (strcmp(newWord, (*first)->word) < 0){
            newNode->next = *first;
            *first = newNode;
        }
        else{
            Node* prev = *first;
            Node* cur = (*first)->next;

            while (cur != NULL && strcmp(newWord, cur->word) >= 0){
                prev = cur;
                cur = cur->next;
            }

            newNode->next = cur;
            prev->next = newNode;
        }
    }
}

void printList(Node* first){
    Node* cur = first;

    if (cur == NULL){
        printf("The list is empty.\n");
    }
    else{
        while (cur != NULL){
            printf("%s\n", cur->word);
            cur = cur->next;
        }
    }
}

void deleteList(Node** first){
    Node* cur = *first;
    Node* temp = NULL;

    while (cur != NULL){
        temp = cur;
        cur = cur->next;
        delete temp;
    }

    *first = NULL;
}

int main(){
    FILE* f = NULL;
    char fileName[260];
    char line[1000];
    Node* first = NULL;

    int vowelCount = 0;
    int consonantCount = 0;

    printf("Input file name: ");
    fgets(fileName, sizeof(fileName), stdin);
    fileName[strcspn(fileName, "\n")] = '\0';

    f = fopen(fileName, "r");

    if (f == NULL){
        printf("Error: cannot open file.\n");
        return 1;
    }

    if (fgets(line, sizeof(line), f) == NULL){
        printf("Error: file is empty.\n");
        fclose(f);
        return 1;
    }

    fclose(f);

    line[strcspn(line, "\n")] = '\0';

    if (strlen(line) == 0){
        printf("Error: input string is empty.\n");
        return 1;
    }

    printf("\nInput string:\n%s\n", line);

    char* token = strtok(line, " ");

    while (token != NULL){
        if (strlen(token) > 0){
            insertSorted(&first, token);
        }

        token = strtok(NULL, " ");
    }

    printf("\nSorted singly linked list:\n");
    printList(first);

    Node* cur = first;

    while (cur != NULL){
        char firstChar = cur->word[0];

        if (isalpha((unsigned char)firstChar)){
            if (isVowel(firstChar)){
                vowelCount++;
            }
            else{
                consonantCount++;
            }
        }

        cur = cur->next;
    }

    printf("\nWords starting with a vowel: %d\n", vowelCount);
    printf("Words starting with a consonant: %d\n", consonantCount);

    if (vowelCount > consonantCount){
        printf("There are more words starting with a vowel.\n");
    }
    else{
        if (consonantCount > vowelCount){
            printf("There are more words starting with a consonant.\n");
        }
        else{
            printf("The number of words starting with a vowel and a consonant is equal.\n");
        }
    }

    deleteList(&first);

    printf("\nMemory has been freed.\n");

    return 0;
}