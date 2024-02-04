#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    int value;
    int weight;
} Item;

typedef struct
{
    int rows;
    int cols;
    int32_t *arr_ptr;
} Matrix_t;

int get_value(Matrix_t *matrix, int i, int j)
{
    // access by V_i_w[i * W + j]
    int c = matrix->cols;

    return matrix->arr_ptr[i * c + j];
}

void set_value(Matrix_t *matrix, int i, int j, int value)
{
    int c = matrix->cols;
    matrix->arr_ptr[i * c + j] = value;
}

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

void calc_value_mt(Matrix_t *matrix, Item *items)
{
    // setting the first row
    for (int w = 0; w < matrix->cols; w++)
    {
        set_value(matrix, 0, w, 0);
    }

    // setting the first col
    for (int i = 0; i < matrix->rows; i++)
        set_value(matrix, i, 0, 0);

    // where N is the number of items
    for (int i = 1; i < matrix->rows; i++)
    {
        // where W is the max capacity of the sack
        for (int w = 1; w < matrix->cols; w++)
        {
            if (w < items[i - 1].weight)
            {
                int x = get_value(matrix, i - 1, w);
                set_value(matrix, i, w, x);
            }
            else
            { // Fits
                int32_t wout_item = get_value(matrix, i - 1, w - items[i - 1].weight) + items[i - 1].value;
                int32_t with_item = get_value(matrix, i - 1, w);

                int m = max(with_item, wout_item);
                set_value(matrix, i, w, m);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        perror("Quantidade de argumentos insuficientes!");

    FILE *input_fp = fopen(argv[1], "r");
    uint32_t N, C;

    if (input_fp == NULL)
        printf("Nao foi possivel abrir o arquivo!");

    fscanf(input_fp, "%d %d", &N, &C);

    // Items
    Item *items = (Item *)malloc(sizeof(Item) * N);

    // setting Item`s values up
    for (int i = 0; i < N; i++)
        fscanf(input_fp, "%d", &items[i].value);

    // setting Item`s weight up
    for (int i = 0; i < N; i++)
        fscanf(input_fp, "%d", &items[i].weight);

    // N = rows (i); W = cols (j)
    Matrix_t V_i_w = {
        .cols = C + 1,
        .rows = N + 1,
        .arr_ptr = (int32_t *)malloc((C + 1) * (N + 1) * sizeof(int32_t)),
    };

    calc_value_mt(&V_i_w, items);

    for (int i = 0; i < V_i_w.rows; i++)
    {
        for (int j = 0; j < V_i_w.cols; j++)
        {
            int x = get_value(&V_i_w, i, j);
            printf("%*d ", 3, x);
        }
        printf("\n");
    }
}
