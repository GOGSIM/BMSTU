/*
с клавиатуры вводятся целые числа. Организовать список с одновременным удалением повторяющихся чисел. 
Вывести печать полученный список. Из полученного списка удалить максимальное число. 
Вывести на печать найденное число и новый список.
*/    

#include <stdio.h>

struct Node {
    int value;
    Node *next;
};

int exists(Node *head, int x) {
    Node *p = head;
    while (p != NULL) {
        if (p->value == x) {
            return 1;  
        }
        p = p->next;
    }
    return 0;           
}

void append_unique(Node *&head, int x) {
    if (exists(head, x)) {
        return; 
    }

    Node *node = new Node; 
    node->value = x;
    node->next = NULL;

    if (head == NULL) {
        head = node;        
    } else {
        Node *p = head;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = node;      
    }
}

void print_list(Node *head) {
    Node *p = head;
    while (p != NULL) {
        printf("%d ", p->value);
        p = p->next;
    }
    printf("\n");
}

void delete_max(Node *&head, int *maxValue) {
    if (head == NULL) {
        return;
    }

    Node *maxNode = head;
    Node *prevMax = NULL;

    Node *prev = NULL;
    Node *cur = head;

    while (cur != NULL) {
        if (cur->value > maxNode->value) {
            maxNode = cur;
            prevMax = prev;
        }
        prev = cur;
        cur = cur->next;
    }

    *maxValue = maxNode->value;

    if (prevMax == NULL) {
        head = head->next;
    } else {
        prevMax->next = maxNode->next;
    }

    delete maxNode; 
}

void free_list(Node *&head) {
    Node *p = head;
    while (p != NULL) {
        Node *next = p->next;
        delete p;
        p = next;
    }
    head = NULL;
}

int main() {
    int n;
    printf("Enter the number of integers: ");
    scanf("%d", &n);

    Node *head = NULL;

    printf("Enter %d integers:\n", n);
    for (int i = 0; i < n; i++) {
        int x;
        scanf("%d", &x);
        append_unique(head, x);   
    }

    printf("\nList without duplicates:\n");
    print_list(head);

    int maxValue;
    delete_max(head, &maxValue);

    printf("\nMaximum number: %d\n", maxValue);

    printf("List after removing the maximum number:\n");
    print_list(head);

    free_list(head); 

    return 0;
}
