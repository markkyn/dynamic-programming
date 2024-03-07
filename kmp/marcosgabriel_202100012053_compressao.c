#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

typedef char *Gene_t;
typedef struct
{
    char *id;
    int gene_count;
    Gene_t *genes;

} Disease_t;

int32_t R_len = 0;
void inserir(int32_t *R, int32_t pos)
{
    R[R_len] = pos;
}

void calcular_tabela(int32_t *k, char *P)
{
    /* Args
        P = Pattern
        k = Transactions
    */

    for (int32_t i = 1, j = -1; i < strlen(P); i++)
    {
        while (j >= 0 && P[j + 1] != P[i])
            j = k[j];

        if (P[j + 1] == P[i])
            j++;

        k[i] = j;
    }
}

void KMP(int32_t *k, int32_t *R, char *T, char *P)
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

        if (P[j + 1] == T[i])
            j++;

        if (j == m - 1)
        {
            inserir(R, i - m + 1);
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
    Disease_t *disease;
    int32_t disease_count;

    /* Capture */
    fscanf(input_fp, "%ld \n", &sub_size);
    fscanf(input_fp, "%[^\n] ", dna);
    fscanf(input_fp, "%d \n", &disease_count);

    printf("%s\n", dna);

    disease = (Disease_t *)malloc(sizeof(Disease_t) * disease_count);
    for (int d = 0; d < disease_count; d++)
    {
        disease[d].id = (char *)malloc(sizeof(char) * 40); // Arbitrary Size
        fscanf(input_fp, "%s %d", disease[d].id, &disease[d].gene_count);

        disease[d].genes = (char **)malloc(sizeof(char *) * disease[d].gene_count);
        for (int g = 0; g < disease[d].gene_count; g++)
        {
            disease[d].genes[g] = (char *)malloc(sizeof(char) * 40); // Arbitrary Size
            fscanf(input_fp, " %s", disease[d].genes[g]);
            printf("%s \n", disease[d].genes[g]);
        }
        printf("\n");
    }
    /* -------- */
    /* KMP */
    Gene_t P;   // Patern
    int32_t *R; // Result

}