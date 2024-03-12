#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>

#define ARBITRARY_SIZE 255

typedef struct
{
    char *P;
    int32_t *k;
    int32_t matches;
    int32_t percent;
} Gene_t;

typedef struct
{
    char *id;
    int32_t gene_count;
    Gene_t *genes;
    int32_t active_genes_count;
    int32_t percent;
} Disease_t;

typedef struct
{
    int d;               // Índice da doença
    Disease_t *diseases; // Ponteiro para o array de doenças
    char *dna;           // Sequência de DNA
    int sub_size;        // Tamanho do substring para o KMP
} ThreadArgs;

void inserir(int32_t *R, int32_t *R_len, int32_t value)
{
    R[(*R_len)++] = value; // its position
}

void merge(Disease_t arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // Cria arrays temporários
    Disease_t *L = (Disease_t *)malloc(n1 * sizeof(Disease_t));
    Disease_t *R = (Disease_t *)malloc(n2 * sizeof(Disease_t));

    // Copia os dados para os arrays temporários L[] e R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // Merge dos arrays temporários de volta em arr[l..r] em ordem decrescente
    i = 0; // Índice inicial do primeiro subarray
    j = 0; // Índice inicial do segundo subarray
    k = l; // Índice inicial do subarray mergeado
    while (i < n1 && j < n2)
    {
        if (L[i].percent >= R[j].percent)
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copia os elementos restantes de L[], se houver
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copia os elementos restantes de R[], se houver
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }

    // Libera a memória alocada para os arrays temporários
    free(L);
    free(R);
}

void mergeSort(Disease_t arr[], int l, int r)
{
    if (l < r)
    {
        // Encontra o ponto médio para dividir o array
        int m = l + (r - l) / 2;

        // Ordena as primeiras e segundas metades
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        // Merge as metades ordenadas
        merge(arr, l, m, r);
    }
}

void calcular_tabela(int32_t *k, char *P)
{
    /* Args
        P = Pattern
        k = Transactions
    */
    // Inicialização

    ////printf("P = ");
    for (int32_t i = 0; i < strlen(P); i++)
    {
        k[i] = -1;
        ////printf("%*c ", 2, P[i]);
    }
    ////printf("\n");

    for (int32_t i = 1, j = -1; i < strlen(P); i++)
    {

        while (j >= 0 && P[j + 1] != P[i])
            j = k[j];

        if (P[j + 1] == P[i])
            j++;

        k[i] = j;
    }
    /*
   //printf("k = ");
    for (int32_t i = 0; i < strlen(P); i++)
    {
       //printf("%*d ", 2, k[i]);
    }
   //printf("\n");
        P  = G  T  C
        k  =   -1 -1
    */
}

void KMP(char *T, char *P, int32_t sub_len, Gene_t *gene)
{
    /* Args
        int32_t *k => Transactions
        int32_t *R => Transaction Result;
        char *T => Text String
        char *P => Pattern String;
    */
    int32_t n = strlen(T), m = strlen(P);
    // printf("%d\n", n);
    if (m == 0)
        return;

    // printf("%d\n", m);
    calcular_tabela(gene->k, P);

    for (int32_t i = 0, j = -1, l = 0; i < n || j == m - 1; i++)
    {
        // Break Match
        while (j >= l && P[j + 1] != T[i])
        {
            // l += j;
            j = gene->k[j];

            ////printf("[BreakAt] i = %d | j = %d\n", i, j);
        }

        // Next State = Char Match!
        if (P[j + 1] == T[i])
        {
            j++;
            ////printf("[CharMatch] i = %2d (%c) | j = %d (%c) next = %c\n", i, T[i], j, P[j], P[j + 1]);
        }

        // FullMatch!
        if (j + 1 >= sub_len + l && (j == m - 1 || P[0] != T[i + 1]))
        {
            gene->matches += j + 1;
            l = j + 1;
        }
    }
    return;
}

void *process_disease(void *args)
{
    ThreadArgs *data = (ThreadArgs *)args;
    int d = data->d;
    Disease_t *diseases = data->diseases;
    char *dna = data->dna;
    int sub_size = data->sub_size;

    for (int g = 0; g < diseases[d].gene_count; g++)
    {
        KMP(dna, diseases[d].genes[g].P, sub_size, &diseases[d].genes[g]);
        if ((((double)diseases[d].genes[g].matches / (double)strlen(diseases[d].genes[g].P)) * 100) >= 90)
            diseases[d].active_genes_count++;
    }

    diseases[d].percent = round(((double)diseases[d].active_genes_count / (double)diseases[d].gene_count) * 100);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        perror("Quantidade de argumentos insuficientes!");

    FILE *input_fp = fopen(argv[1], "r");
    FILE *output_fp = fopen(argv[2], "w");

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

    // printf("%s\n", dna);

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
            diseases[d].genes[g].P = malloc(40000 * sizeof(char));
            diseases[d].genes[g].k = malloc(40000 * sizeof(char));
            fscanf(input_fp, " %s", diseases[d].genes[g].P);
        }
    }

    /* KMP */
    pthread_t threads[disease_count];
    ThreadArgs args[disease_count];

    //   for each disease:
    for (int d = 0; d < disease_count; d++)
    {
        args[d] = (ThreadArgs){d, diseases, dna, sub_size};
        pthread_create(&threads[d], NULL, process_disease, (void *)&args[d]);
    }

    for (int d = 0; d < disease_count; d++)
    {
        pthread_join(threads[d], NULL);
    }

    printf("\nSorting\n");
    mergeSort(diseases, 0, disease_count - 1);

    for (int32_t d = 0; d < disease_count; d++)
        fprintf(output_fp, "%s->%d%%\n", diseases[d].id, diseases[d].percent);
}