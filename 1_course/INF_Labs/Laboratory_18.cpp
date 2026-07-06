/*
построить двоичное дерево поиска из букв вводимой строки. 
Вывести его на экран. Определить все гласные буквы и их количество. 
Удалить из дерева эти буквы. Вывести оставшиеся элементы дерева. 
Сравнить время удаления гласных букв из дерева и из строки. 
Вывести на экран всю необходимую информацию с комментариями.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

struct Node
{
    char data;
    Node* left;
    Node* right;
};

int is_vowel(char c)
{
    c = (char)tolower((unsigned char)c);
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

Node* create_node(char c)
{
    Node* p = (Node*)malloc(sizeof(Node));
    p->data = c;
    p->left = NULL;
    p->right = NULL;
    return p;
}

Node* insert_node(Node* root, char c)
{
    if (root == NULL)
        return create_node(c);
    if (c < root->data)
        root->left = insert_node(root->left, c);
    else
        root->right = insert_node(root->right, c);
    return root;
}

void build_tree(Node** root, char s[])
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (isalpha((unsigned char)s[i]))
            *root = insert_node(*root, (char)tolower((unsigned char)s[i]));
        i++;
    }
}

void preorder(Node* root)
{
    if (root != NULL)
    {
        printf("%c ", root->data);
        preorder(root->left);
        preorder(root->right);
    }
}

void inorder(Node* root)
{
    if (root != NULL)
    {
        inorder(root->left);
        printf("%c ", root->data);
        inorder(root->right);
    }
}

void postorder(Node* root)
{
    if (root != NULL)
    {
        postorder(root->left);
        postorder(root->right);
        printf("%c ", root->data);
    }
}

void print_tree(Node* root, int space)
{
    int i;
    if (root != NULL)
    {
        space += 5;
        print_tree(root->right, space);
        printf("\n");
        for (i = 5; i < space; i++)
            printf(" ");
        printf("%c\n", root->data);
        print_tree(root->left, space);
    }
}

int count_nodes(Node* root)
{
    if (root == NULL)
        return 0;
    return count_nodes(root->left) + 1 + count_nodes(root->right);
}

int depth_tree(Node* root)
{
    int l, r;
    if (root == NULL)
        return 0;
    l = depth_tree(root->left);
    r = depth_tree(root->right);
    if (l > r)
        return l + 1;
    return r + 1;
}

Node* find_min(Node* root)
{
    while (root->left != NULL)
        root = root->left;
    return root;
}

Node* delete_node(Node* root, char key)
{
    Node* t;
    if (root == NULL)
        return NULL;
    if (key < root->data)
        root->left = delete_node(root->left, key);
    else
    {
        if (key > root->data)
            root->right = delete_node(root->right, key);
        else
        {
            if (root->left == NULL)
            {
                t = root->right;
                free(root);
                return t;
            }
            if (root->right == NULL)
            {
                t = root->left;
                free(root);
                return t;
            }
            t = find_min(root->right);
            root->data = t->data;
            root->right = delete_node(root->right, t->data);
        }
    }
    return root;
}

void delete_vowels_tree(Node** root, char s[])
{
    int i = 0;
    char c;
    while (s[i] != '\0')
    {
        if (isalpha((unsigned char)s[i]))
        {
            c = (char)tolower((unsigned char)s[i]);
            if (is_vowel(c))
                *root = delete_node(*root, c);
        }
        i++;
    }
}

void remove_vowels_string(char s[])
{
    int i = 0, j = 0;
    while (s[i] != '\0')
    {
        if (!(isalpha((unsigned char)s[i]) && is_vowel(s[i])))
        {
            s[j] = s[i];
            j++;
        }
        i++;
    }
    s[j] = '\0';
}

void count_vowels(char s[], int* total)
{
    int i = 0;
    *total = 0;
    while (s[i] != '\0')
    {
        if (isalpha((unsigned char)s[i]) && is_vowel(s[i]))
            (*total)++;
        i++;
    }
}

void free_tree(Node* root)
{
    if (root != NULL)
    {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

void print_info(Node* root)
{
    printf("\nPreorder: ");
    preorder(root);
    printf("\nInorder: ");
    inorder(root);
    printf("\nPostorder: ");
    postorder(root);
    printf("\n\nTree:\n");
    print_tree(root, 0);
    printf("\nDepth: %d\n", depth_tree(root));
    printf("Nodes: %d\n", count_nodes(root));
}

int main()
{
    char s[256], s2[256];
    Node* root = NULL;
    int vowels;
    clock_t t1, t2, t3, t4;

    printf("Input string: ");
    fgets(s, 256, stdin);
    s[strcspn(s, "\n")] = '\0';
    strcpy(s2, s);

    build_tree(&root, s);

    printf("\nInitial tree:\n");
    print_info(root);

    count_vowels(s, &vowels);
    printf("\nVowels count: %d\n", vowels);

    t1 = clock();
    delete_vowels_tree(&root, s);
    t2 = clock();

    t3 = clock();
    remove_vowels_string(s2);
    t4 = clock();

    printf("\nTree after deleting vowels:\n");
    print_info(root);

    printf("\nString after deleting vowels:\n%s\n", s2);

    printf("\nTime delete from tree: %.10f sec\n", (double)(t2 - t1) / CLOCKS_PER_SEC);
    printf("Time delete from string: %.10f sec\n", (double)(t4 - t3) / CLOCKS_PER_SEC);

    free_tree(root);
    return 0;
}