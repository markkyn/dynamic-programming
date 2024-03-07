#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define ARBITRARY_SIZE 40

typedef char *Gene_t;
typedef struct
{
    char *id;
    int32_t gene_count;
    Gene_t *genes;
    int32_t *R;
    int32_t R_count;
} Disease_t;

int32_t R_len = 0;
void inserir(int32_t *R, int32_t *R_count, int32_t value)
{
    R[*R_count] = value; // its position
    (*R_count)++;
}

void calcular_tabela(int32_t *k, char *P)
{
    /* Args
        P = Pattern
        k = Transactions
    */
    // Inicialização

    printf("P = ");
    for (int32_t i = 0; i < strlen(P); i++)
    {
        k[i] = -1;
        printf("%*c ", 2, P[i]);
    }
    printf("\n");

    for (int32_t i = 1, j = -1; i < strlen(P); i++)
    {

        while (j >= 0 && P[j + 1] != P[i])
            j = k[j];

        if (P[j + 1] == P[i])
            j++;

        k[i] = j;
    }

    printf("k = ");
    for (int32_t i = 0; i < strlen(P); i++)
    {
        printf("%*d ", 2, k[i]);
    }
    printf("\n");
    /*
        P  = G  T  C
        k  =   -1 -1
    */
}

void KMP(int32_t *k, int32_t *R, int32_t *R_count, char *T, Gene_t P)
{
    /* Args
        int32_t *k => Transactions
        int32_t *R => Transaction Result;
        char *T => Text String
        char *P => Pattern String;
    */

    int32_t n = strlen(T), m = strlen(P);

    calcular_tabela(k, P);

    for (int32_t i = 0, j = -1; i < n; i++)
    {
        while (j >= 0 && P[j + 1] != T[i])
            j = k[j];

        if (P[j + 1] == T[i]) // Correspondência
            j++;

        if (j == m - 1)
        {
            inserir(R, R_count, i - m + 1);
            j = k[j];
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        perror("Quantidade de argumentos insuficientes!");

    FILE *input_fp = fopen(argv[1], "r");

    struct stat sb;
    stat(argv[1], &sb);

    /* File Formats */
    char *dna = malloc(sb.st_size);
    size_t sub_size;
    Disease_t *diseases;
    int32_t disease_count;

    /* Capture */
    fscanf(input_fp, "%ld \n", &sub_size);
    fscanf(input_fp, "%[^\n] ", dna);
    fscanf(input_fp, "%d \n", &disease_count);

    printf("%s\n", dna);

    diseases = (Disease_t *)malloc(sizeof(Disease_t) * disease_count);
    for (int d = 0; d < disease_count; d++)
    {
        diseases[d].id = (char *)malloc(sizeof(char) * ARBITRARY_SIZE); // Arbitrary Size
        fscanf(input_fp, "%s %d", diseases[d].id, &diseases[d].gene_count);

        diseases[d].R = (int32_t *)malloc(sizeof(int32_t) * ARBITRARY_SIZE);
        diseases[d].R_count = 0;
        diseases[d].genes = (char **)malloc(sizeof(char *) * diseases[d].gene_count);

        for (int g = 0; g < diseases[d].gene_count; g++)
        {
            diseases[d].genes[g] = (char *)malloc(sizeof(char) * ARBITRARY_SIZE); // Arbitrary Size
            fscanf(input_fp, " %s", diseases[d].genes[g]);
            printf("%s \n", diseases[d].genes[g]);
        }
        printf("\n");
    }

    /* KMP */
    int32_t *k = (int32_t *)malloc(sizeof(int32_t) * ARBITRARY_SIZE);

    printf("str_len(dna) = %ld\n", strlen(dna));
    for (int d = 0; d < disease_count; d++)
    {
        for (int g = 0; g < diseases[d].gene_count; g++)
        {
            KMP(k, diseases[d].R, &diseases[d].R_count, dna, diseases[d].genes[g]);

            printf("R = ");
            for (int i = 1; i < diseases[d].R_count; i++)
                printf("%*d ", 2, diseases[d].R[i]);
            printf("\n\n");
        }
    }
}