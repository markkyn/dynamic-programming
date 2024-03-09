#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

#define ARBITRARY_SIZE 255

typedef struct
{
    char *P;
    int32_t *R;
    int32_t R_len;

} Gene_t;

typedef struct
{
    char *id;
    int32_t gene_count;
    Gene_t *genes;
    int32_t percetage;
} Disease_t;

void inserir(int32_t *R, int32_t *R_len, int32_t value)
{
    R[(*R_len)++] = value; // its position
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

bool KMP(int32_t *k, int32_t *R, int32_t *R_len, char *T, char *P)
{
    /* Args
        int32_t *k => Transactions
        int32_t *R => Transaction Result;
        char *T => Text String
        char *P => Pattern String;
    */
    int32_t n = strlen(T), m = strlen(P);

    printf("%d\n", m);
    calcular_tabela(k, P);

    for (int32_t i = 0, j = -1; i < n; i++)
    {
        while (j >= 0 && P[j + 1] != T[i]) // Back
            j = k[j];

        if (P[j + 1] == T[i]) // Next State = Char Match!
            j++;

        if (j == (m - 1))
        {
            // inserir(R, R_len, i - m + 1);
            j = k[j];

            return true; // Match!!
        }
    }
    return false;
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
    // for each disease
    for (int d = 0; d < disease_count; d++)
    {
        diseases[d].id = (char *)malloc(sizeof(char) * ARBITRARY_SIZE); // Arbitrary Size
        fscanf(input_fp, "%s %d", diseases[d].id, &diseases[d].gene_count);

        diseases[d].genes = (Gene_t *)malloc(sizeof(Gene_t) * diseases[d].gene_count);

        // for each gene in disease:
        for (int g = 0; g < diseases[d].gene_count; g++)
        {
            diseases[d].genes[g].R = (int32_t *)malloc(sizeof(int32_t) * ARBITRARY_SIZE);
            diseases[d].genes[g].P = (char *)malloc(sizeof(char) * ARBITRARY_SIZE); // Arbitrary Size
            fscanf(input_fp, " %s", diseases[d].genes[g].P);
            printf("%s \n", diseases[d].genes[g].P);
        }
        printf("\n");
    }

    /* KMP */
    int32_t *k = (int32_t *)malloc(sizeof(int32_t) * ARBITRARY_SIZE);

    printf("str_len(dna) = %ld\n", strlen(dna));
    //  for each disease:
    for (int d = 0; d < disease_count; d++)
    {
        //  for each gene:
        int32_t matches = 0;
        for (int g = 0; g < diseases[d].gene_count; g++)
        {
            if (KMP(k, diseases[d].genes[g].R, &diseases[d].genes[g].R_len, dna, diseases[d].genes[g].P))
            { // In case there is a Match
                matches++;
            }

            printf("R = ");
            for (int32_t r = 0; r < diseases[d].genes[g].R_len; r++)
                printf("%d ", diseases[d].genes[g].R[r]);
            printf("\n");
        }
        diseases[d].percetage = ((double)matches / (double)diseases[d].gene_count) * 100;
        printf("\t[Disease = %s] match = %d\n", diseases[d].id, diseases[d].percetage);
    }
}