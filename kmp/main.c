#include <stdint.h>
#include <stdio.h>

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

int main(int argc, char *argv[])
{
    if (argc < 3)
        perror("Quantidade de argumentos insuficientes!");

    FILE *input_fp = fopen(argv[1], "r");
}