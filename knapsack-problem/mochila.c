#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    int id;
    int value;
    int weight;
} Item_t;

typedef struct
{
    Item_t *items;
    int count;
} Knapsack_t;

typedef struct
{
    int rows;
    int cols;
    int32_t *arr_ptr;
} Matrix_t;

void add_item(Knapsack_t *sack, Item_t item)
{
    sack->items[sack->count] = item;
    sack->count++;
}

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

void calc_value_mt(Matrix_t *matrix, Item_t *items)
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

Knapsack_t select_items(Matrix_t *matrix, Item_t *items)
{

    Knapsack_t sack = {
        .count = 0,
        .items = (Item_t *)malloc(sizeof(Item_t) * matrix->rows)};

    for (int i = matrix->rows - 1; i > 0;)
    {
        for (int j = matrix->cols - 1; j > 0;)
        {
            if (get_value(matrix, i, j) != get_value(matrix, i - 1, j))
            {
                printf("Item[%d]", i);

                add_item(&sack, items[i]);
                i = i - 1;
                j = j - items[i].weight;
            }
            else
            {
                i = i - 1;
            }
        }
    }
    return sack;
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
    Item_t *items = (Item_t *)malloc(sizeof(Item_t) * N);

    // setting Item_t`s values up
    for (int i = 0; i < N; i++)
    {
        items->id = i;
        fscanf(input_fp, "%d", &items[i].value);
    }

    // setting Item_t`s weight up
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

    Knapsack_t sack = select_items(&V_i_w, items);
    for (int i = 0; i < sack.count; i++)
    {
       // printf("Item[%d] -> %dkg/R$ %d,00\n", sack.items->id, sack.items[i].weight, sack.items[i].value);
    }
}
