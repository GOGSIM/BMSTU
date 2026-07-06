/*
создать бинарный файл F, состоящий из целых чисел. 
Переписать из файла F в файл G все нечетные числа.
*/
    
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *F, *G;
    int n;

    F = fopen("F.bin", "wb");
    if (!F) {
        printf("Error: cannot create file F.bin\n");
        return 1;
    }

    printf("Enter how many integers to write into file F: ");
    scanf("%d", &n);

    int value;
    printf("Enter %d integers:\n", n);

    for (int i = 0; i < n; i++) {
        printf("value[%d] = ", i);
        scanf("%d", &value);
        fwrite(&value, sizeof(int), 1, F);
    }

    fclose(F);
    printf("File F.bin created successfully.\n\n");

    F = fopen("F.bin", "rb");
    G = fopen("G.bin", "wb");

    if (!F || !G) {
        printf("Error: cannot open F.bin or G.bin\n");
        return 1;
    }

    printf("Reading numbers from F.bin and writing odd numbers to G.bin:\n");

    while (fread(&value, sizeof(int), 1, F) == 1) {
        printf("Read: %d -> ", value);
        if (value % 2 != 0) {
            fwrite(&value, sizeof(int), 1, G);
            printf("written to G (odd)\n");
        } else {
            printf("skipped (even)\n");
        }
    }

    fclose(F);
    fclose(G);

    printf("\nProcess completed.\n");

    return 0;
}
