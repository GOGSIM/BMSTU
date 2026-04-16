/*
Part A
Сформировать бинарный файл данных типа «структура», содержащий следующие сведения о компьютерах: 
тип микропроцессора, объем памяти, объем винчестера, цена. Программа должна формировать файл, 
добавлять и удалять данные, а также воспринимать каждый из перечисленных запросов и давать на него ответ. 
1) Определить характеристики компьютеров, цена которых не превышает заданную с клавиатуры.
2) Определить типы микропроцессоров и цены всех компьютеров с памятью не менее заданного объема.
3) Определить цены всех компьютеров с заданным типом микропроцессора, 
обладающих одновременно памятью и винчестерами не менее заданных объемов.
При обработке файла и выборе функций использовать консольное меню.
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

struct computer
{
    char cpu[40];
    int memory;
    int hdd;
    float price;
};

void print_record(computer c, int k)
{
    printf("\nRecord %d\n", k);
    printf("CPU: %s\n", c.cpu);
    printf("Memory: %d\n", c.memory);
    printf("HDD: %d\n", c.hdd);
    printf("Price: %.2f\n", c.price);
}

void print_file(FILE* f)
{
    computer c;
    int k = 0;

    rewind(f);
    printf("\n===== FILE =====\n");
    while (fread(&c, sizeof(c), 1, f) == 1)
    {
        k++;
        print_record(c, k);
    }

    if (k == 0)
        printf("File is empty.\n");
}

void add_record(FILE* f)
{
    computer c;

    printf("Input CPU type: ");
    scanf("%39s", c.cpu);
    printf("Input memory: ");
    scanf("%d", &c.memory);
    printf("Input HDD: ");
    scanf("%d", &c.hdd);
    printf("Input price: ");
    scanf("%f", &c.price);

    fseek(f, 0, SEEK_END);
    fwrite(&c, sizeof(c), 1, f);
    fflush(f);

    printf("Record added.\n");
}

int count_records(FILE* f)
{
    computer c;
    int k = 0;

    rewind(f);
    while (fread(&c, sizeof(c), 1, f) == 1)
        k++;

    return k;
}

FILE* delete_record(FILE* f, const char fname[], int num)
{
    FILE* t;
    computer c;
    int k = 0;

    t = fopen("temp.dat", "wb");
    rewind(f);

    while (fread(&c, sizeof(c), 1, f) == 1)
    {
        k++;
        if (k != num)
            fwrite(&c, sizeof(c), 1, t);
    }

    fclose(f);
    fclose(t);
    remove(fname);
    rename("temp.dat", fname);

    return fopen(fname, "a+b");
}

void find1(FILE* f)
{
    computer c;
    float p;
    int k = 0;

    printf("Input max price: ");
    scanf("%f", &p);

    rewind(f);
    printf("\nComputers with price <= %.2f\n", p);
    while (fread(&c, sizeof(c), 1, f) == 1)
    {
        if (c.price <= p)
        {
            k++;
            print_record(c, k);
        }
    }

    if (k == 0)
        printf("Nothing found.\n");
}

void find2(FILE* f)
{
    computer c;
    int m;
    int k = 0;

    printf("Input min memory: ");
    scanf("%d", &m);

    rewind(f);
    printf("\nCPU types and prices for computers with memory >= %d\n", m);
    while (fread(&c, sizeof(c), 1, f) == 1)
    {
        if (c.memory >= m)
        {
            k++;
            printf("%d) CPU: %s  Price: %.2f\n", k, c.cpu, c.price);
        }
    }

    if (k == 0)
        printf("Nothing found.\n");
}

void find3(FILE* f)
{
    computer c;
    char cpu[40];
    int m, h;
    int k = 0;

    printf("Input CPU type: ");
    scanf("%39s", cpu);
    printf("Input min memory: ");
    scanf("%d", &m);
    printf("Input min HDD: ");
    scanf("%d", &h);

    rewind(f);
    printf("\nPrices of computers with given CPU, memory >= %d, HDD >= %d\n", m, h);
    while (fread(&c, sizeof(c), 1, f) == 1)
    {
        if (strcmp(c.cpu, cpu) == 0 && c.memory >= m && c.hdd >= h)
        {
            k++;
            printf("%d) Price: %.2f\n", k, c.price);
        }
    }

    if (k == 0)
        printf("Nothing found.\n");
}

void menu_search(FILE* f)
{
    int k = 0;

    while (k != 4)
    {
        printf("\n===== SEARCH MENU =====\n");
        printf("1. Computers with price <= given\n");
        printf("2. CPU types and prices with memory >= given\n");
        printf("3. Prices by CPU type, memory and HDD\n");
        printf("4. Exit search menu\n");
        printf("Choose: ");
        scanf("%d", &k);

        if (k == 1) find1(f);
        if (k == 2) find2(f);
        if (k == 3) find3(f);
        if (k == 4) printf("Exit from search menu.\n");
        if (k < 1 || k > 4) printf("Wrong choice.\n");
    }
}

int main()
{
    const char fname[] = "data.dat";
    FILE* f = fopen(fname, "w+b");
    int k = 0, n, cnt;

    while (k != 5)
    {
        printf("\n===== MAIN MENU =====\n");
        printf("1. Add record\n");
        printf("2. Delete record by number\n");
        printf("3. Print file\n");
        printf("4. Search\n");
        printf("5. Exit\n");
        printf("Choose: ");
        scanf("%d", &k);

        if (k == 1)
            add_record(f);

        if (k == 2)
        {
            print_file(f);
            cnt = count_records(f);
            if (cnt > 0)
            {
                printf("Input record number to delete: ");
                scanf("%d", &n);
                if (n >= 1 && n <= cnt)
                {
                    f = delete_record(f, fname, n);
                    printf("Record deleted.\n");
                }
                else
                    printf("Wrong record number.\n");
            }
        }

        if (k == 3)
            print_file(f);

        if (k == 4)
            menu_search(f);

        if (k == 5)
            printf("End of program.\n");

        if (k < 1 || k > 5)
            printf("Wrong choice.\n");
    }

    fclose(f);
    return 0;
}

/*
Part B
Составить программу, используя рекурсивную процедуру (функцию).
Упорядочить массив из m действительных чисел методом “быстрой сортировки”: 
выбрать средний элемент и переставить элементы массива так, 
чтобы слева от выбранного остались только те элементы, которые меньше него, 
а справа те, которые больше. 
Тем самым выбранный элемент оказывается на своем месте. 
После этого описанный алгоритм рекурсивно применяется к левой и правой частям массива.    
Составить тестирующую программу, используя рекурсивную процедуру (функцию).
Оттестировать программу на различных m. 
Определить, как зависит количество вызовов программы от величины m. 
Вывести на экран всю необходимую информацию. 
В отчете привести дерево рекурсии для небольшого m.
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_array(double a[], int n)
{
    int i;
    for (i = 0; i < n; i++)
        printf("%8.3lf", a[i]);
    printf("\n");
}

void input_array(double a[], int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        printf("a[%d] = ", i);
        scanf("%lf", &a[i]);
    }
}

void random_array(double a[], int n)
{
    int i;
    for (i = 0; i < n; i++)
        a[i] = (rand() % 2001 - 1000) / 10.0;
}

void quick_sort(double a[], int left, int right, int* calls)
{
    int i, j;
    double x, t;

    (*calls)++;

    i = left;
    j = right;
    x = a[(left + right) / 2];

    while (i <= j)
    {
        while (a[i] < x)
            i++;

        while (a[j] > x)
            j--;

        if (i <= j)
        {
            t = a[i];
            a[i] = a[j];
            a[j] = t;
            i++;
            j--;
        }
    }

    if (left < j)
        quick_sort(a, left, j, calls);

    if (i < right)
        quick_sort(a, i, right, calls);
}

void test_program()
{
    int n, mode, calls;
    double* a;

    printf("Input number of elements: ");
    scanf("%d", &n);

    a = new double[n];

    printf("1 - keyboard input\n");
    printf("2 - random input\n");
    printf("Choose input mode: ");
    scanf("%d", &mode);

    if (mode == 1)
        input_array(a, n);
    else
        random_array(a, n);

    printf("\nInput array:\n");
    print_array(a, n);

    calls = 0;
    quick_sort(a, 0, n - 1, &calls);

    printf("\nSorted array:\n");
    print_array(a, n);

    printf("\nArray size = %d\n", n);
    printf("Recursion depth in worst case depends on partitioning.\n");
    printf("Number of recursive calls = %d\n", calls);

    delete[] a;
}

void series_test()
{
    int n, i, calls;
    double* a;

    printf("\nDependence of number of calls on m\n");
    printf(" m        calls\n");

    for (n = 2; n <= 10; n++)
    {
        a = new double[n];
        for (i = 0; i < n; i++)
            a[i] = rand() % 1000 / 10.0;

        calls = 0;
        quick_sort(a, 0, n - 1, &calls);

        printf("%2d %12d\n", n, calls);
        delete[] a;
    }
}

int main()
{
    int key;

    srand((unsigned)time(NULL));
    key = -1;

    while (key != 0)
    {
        printf("\n===== MENU =====\n");
        printf("1. Test quick sort\n");
        printf("2. Show dependence of calls on m\n");
        printf("0. Exit\n");
        printf("Choose: ");
        scanf("%d", &key);

        if (key == 1)
            test_program();

        if (key == 2)
            series_test();

        if (key == 0)
            printf("End of program.\n");

        if (key < 0 || key > 2)
            printf("Wrong choice.\n");
    }

    return 0;
}

/*
Part C
Для заданного арифметического выражения построить дерево арифметического разбора, 
написать алгоритм подсчета арифметического выражения, 
организовав ввод переменных с клавиатуры и 
с помощью датчика случайных чисел в соответствии с заданным вариантом. 
Вывести дерево и результат расчета на экран. 
Провести тестирование программы.
Выражение: (3*x + 4) – (y*7 – (-4+2))
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Node
{
    char op;
    double value;
    char var;
    Node* left;
    Node* right;
};

Node* create_value_node(double value)
{
    Node* node = (Node*)malloc(sizeof(Node));
    node->op = '\0';
    node->value = value;
    node->var = '\0';
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* create_var_node(char var)
{
    Node* node = (Node*)malloc(sizeof(Node));
    node->op = '\0';
    node->value = 0;
    node->var = var;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* create_op_node(char op, Node* left, Node* right)
{
    Node* node = (Node*)malloc(sizeof(Node));
    node->op = op;
    node->value = 0;
    node->var = '\0';
    node->left = left;
    node->right = right;
    return node;
}

Node* build_expression_tree()
{
    Node* three = create_value_node(3);
    Node* x = create_var_node('x');
    Node* mul1 = create_op_node('*', three, x);
    Node* four = create_value_node(4);
    Node* left_part = create_op_node('+', mul1, four);

    Node* y = create_var_node('y');
    Node* seven = create_value_node(7);
    Node* mul2 = create_op_node('*', y, seven);

    Node* minus_four = create_value_node(-4);
    Node* two = create_value_node(2);
    Node* plus2 = create_op_node('+', minus_four, two);

    Node* right_part = create_op_node('-', mul2, plus2);

    return create_op_node('-', left_part, right_part);
}

double evaluate(Node* node, double x, double y)
{
    double l, r;

    if (node->op == '\0')
    {
        if (node->var == 'x')
            return x;
        if (node->var == 'y')
            return y;
        return node->value;
    }

    l = evaluate(node->left, x, y);
    r = evaluate(node->right, x, y);

    if (node->op == '+')
        return l + r;
    if (node->op == '-')
        return l - r;
    if (node->op == '*')
        return l * r;
    if (node->op == '/')
        return l / r;

    return 0;
}

void preorder(Node* node)
{
    if (node != NULL)
    {
        if (node->op != '\0')
            printf("%c ", node->op);
        else
        {
            if (node->var != '\0')
                printf("%c ", node->var);
            else
                printf("%.2f ", node->value);
        }
        preorder(node->left);
        preorder(node->right);
    }
}

void inorder(Node* node)
{
    if (node != NULL)
    {
        inorder(node->left);
        if (node->op != '\0')
            printf("%c ", node->op);
        else
        {
            if (node->var != '\0')
                printf("%c ", node->var);
            else
                printf("%.2f ", node->value);
        }
        inorder(node->right);
    }
}

void postorder(Node* node)
{
    if (node != NULL)
    {
        postorder(node->left);
        postorder(node->right);
        if (node->op != '\0')
            printf("%c ", node->op);
        else
        {
            if (node->var != '\0')
                printf("%c ", node->var);
            else
                printf("%.2f ", node->value);
        }
    }
}

int count_nodes(Node* node)
{
    if (node == NULL)
        return 0;
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

int tree_depth(Node* node)
{
    int l, r;
    if (node == NULL)
        return 0;
    l = tree_depth(node->left);
    r = tree_depth(node->right);
    if (l > r)
        return l + 1;
    return r + 1;
}

void print_tree(Node* node, int space)
{
    int i;
    if (node != NULL)
    {
        space += 5;
        print_tree(node->right, space);
        printf("\n");
        for (i = 5; i < space; i++)
            printf(" ");
        if (node->op != '\0')
            printf("%c\n", node->op);
        else
        {
            if (node->var != '\0')
                printf("%c\n", node->var);
            else
                printf("%.0f\n", node->value);
        }
        print_tree(node->left, space);
    }
}

void free_tree(Node* node)
{
    if (node != NULL)
    {
        free_tree(node->left);
        free_tree(node->right);
        free(node);
    }
}

void input_values(double* x, double* y)
{
    printf("Input x: ");
    scanf("%lf", x);
    printf("Input y: ");
    scanf("%lf", y);
}

void random_values(double* x, double* y)
{
    *x = rand() % 21 - 10;
    *y = rand() % 21 - 10;
    printf("Random x = %.2f\n", *x);
    printf("Random y = %.2f\n", *y);
}

void show_info(Node* tree)
{
    printf("\nExpression tree:\n");
    print_tree(tree, 0);
    printf("\nPreorder: ");
    preorder(tree);
    printf("\nInorder: ");
    inorder(tree);
    printf("\nPostorder: ");
    postorder(tree);
    printf("\n\nNumber of nodes: %d\n", count_nodes(tree));
    printf("Depth of tree: %d\n", tree_depth(tree));
}

int main()
{
    Node* tree = build_expression_tree();
    double x = 0, y = 0, result;
    int key = -1, mode;

    srand((unsigned)time(NULL));

    while (key != 0)
    {
        printf("\n===== MENU =====\n");
        printf("1. Input variables and calculate expression\n");
        printf("2. Show tree\n");
        printf("3. Show traversals\n");
        printf("4. Show characteristics\n");
        printf("5. Show all information\n");
        printf("0. Exit\n");
        printf("Choose: ");
        scanf("%d", &key);

        if (key == 1)
        {
            printf("1 - Keyboard input\n");
            printf("2 - Random input\n");
            printf("Choose mode: ");
            scanf("%d", &mode);

            if (mode == 1)
                input_values(&x, &y);
            if (mode == 2)
                random_values(&x, &y);

            result = evaluate(tree, x, y);
            printf("Expression: (3*x + 4) - (y*7 - (-4 + 2))\n");
            printf("Result = %.6f\n", result);
        }

        if (key == 2)
        {
            printf("\nExpression tree:\n");
            print_tree(tree, 0);
            printf("\n");
        }

        if (key == 3)
        {
            printf("\nPreorder: ");
            preorder(tree);
            printf("\nInorder: ");
            inorder(tree);
            printf("\nPostorder: ");
            postorder(tree);
            printf("\n");
        }

        if (key == 4)
        {
            printf("\nNumber of nodes: %d\n", count_nodes(tree));
            printf("Depth of tree: %d\n", tree_depth(tree));
        }

        if (key == 5)
            show_info(tree);

        if (key == 0)
            printf("End of program.\n");

        if (key < 0 || key > 5)
            printf("Wrong choice.\n");
    }

    free_tree(tree);
    return 0;
}