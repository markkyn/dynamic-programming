#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef struct
{
    char *id;
    double value;
    int weight;
    int volume;
    bool allocated;
} Item_t;

typedef struct
{
    char plate[7];
    int weight_cap;
    int volume_cap;
} Vehicle_t;

typedef struct
{
    int32_t volume;
    double value;
    int32_t weight;
    Item_t *items;
    int count;
} Knapsack_t;

typedef struct
{
    int rows;
    int cols;
    int depth;
    double *arr_ptr;
} Matrix_t;

/*
       Z--------Highest Volume && Lowest Weight
      /0 0 0 0 /|
     /0 0 0 0 / |
    o--------Y  |
    |0 0 0 0 |  |
    |0 0 0 0 |  Highest Value ( weight and volume instances )
    |0 0 0 0 | /
    |0 0 0 0 |/
    X--------Lowest Volume && Highest Weight
*/

FILE *input_fp;
FILE *output_fp;
Item_t *items = NULL;
int N, M;
double pending_value  = 0;
int pending_weight = 0;
int pending_volume = 0;

void loadItems(FILE *input_fp, Item_t **head, int32_t M)
{
    items = (Item_t *)malloc(sizeof(Item_t) * (M + 1) * 2);

    printf("\n--------- ITEMS ------------------------\n");
    items[0].value = 0.0;
    items[0].weight = 0;
    items[0].volume = 0;
    for (int i = 1; i <= M; i++)
    {
        items[i].id = (char *)malloc(14 * sizeof(char));
        fscanf(input_fp, "%s %lf %d %d", items[i].id, &items[i].value, &items[i].weight, &items[i].volume);

        items[i].allocated = false;
        pending_value +=  items[i].value;
        pending_weight += items[i].weight;
        pending_volume += items[i].volume;

       // printf("%*d | %s | %*.2f | %*d | %*d |\n", 5, i, items[i].id, 7, items[i].value, 3, items[i].weight, 3, items[i].volume);
    }
    //printf("----------------------------------------\n");
}

void add_item(Knapsack_t *sack, Item_t *item, int index)
{
    sack->items[sack->count] = *item;
    sack->count++;

    sack->value  += item->value;
    sack->weight += item->weight;
    sack->volume += item->volume;

    pending_value  -= item->value;
    pending_weight -= item->weight;
    pending_volume -= item->volume;


    (*item).allocated = true;
}

void rearrangeItems(Item_t *items)
{
    int j = 0;
    for (int i = 0; i < M + 1; i++)
    {
        if (items[i].allocated)
        {
            if (i != j)
            {
                items[j] = items[i];
            }
            j++;
        }
    }
}

void swapItems(Item_t *item1_ptr, Item_t *item2_ptr)
{
    Item_t temp = *item1_ptr;

    *item1_ptr = *item2_ptr;

    *item2_ptr = temp;
}

double get_value(Matrix_t *matrix, int x, int y, int z)
{
    int c = matrix->cols; // Número de colunas
    int r = matrix->rows; // Número de linhas

    return matrix->arr_ptr[z * (r * c) + x * c + y];
}

void set_value(Matrix_t *matrix, int x, int y, int z, double value)
{
    int c = matrix->cols; // Número de colunas
    int r = matrix->rows; // Número de linhas
    int d = matrix->depth;

    // printf("[Set Value] (%d * (r * c) + %d * c + %d) = %d\n", z, x, y, z * (r * c) + x * c + y);
    if (z <= d - 1 && x <= r - 1 && y <= c - 1)
        matrix->arr_ptr[z * (r * c) + x * c + y] = value;
}

double max(double a, double b)
{
    return (a > b) ? a : b;
}

void calc_value_mt(Matrix_t *matrix, Item_t *items, int minimum_vol, int minimum_capw)
{
    for (int v = 0; v < matrix->depth; v++)        // v = Z axis
        for (int i = 1; i < matrix->rows; i++)     // i = X axis
            for (int w = 0; w < matrix->cols; w++) // w = Y axis
            {
                if (w < items[i].weight || v < items[i].volume) // dont fit
                {
                    double value = get_value(matrix, i - 1, w, v);
                    set_value(matrix, i, w, v, value);
                }
                else // fits
                {
                    double value_with_item = get_value(matrix, i - 1, w - items[i].weight, v - items[i].volume) + items[i].value;
                    double value_without_item = get_value(matrix, i - 1, w, v);

                    set_value(matrix, i, w, v, max(value_with_item, value_without_item));
                }
            }
}

Knapsack_t select_items(Matrix_t *matrix, Vehicle_t *vehicle, Item_t *items, int32_t items_count)
{
    int v = vehicle->volume_cap;
    int i = items_count - 1;
    int w = vehicle->weight_cap;

    Knapsack_t knapsack = {
        .value = 0,
        .volume = 0,
        .count = 0,
        .weight = 0,
        .items = (Item_t *)malloc(sizeof(Item_t) * matrix->rows)};

    while (i > 0)
    {
        if (get_value(matrix, i, w, v) != get_value(matrix, i - 1, w, v))
        {
            add_item(&knapsack, &items[i], i);
            // printf("[Add Item] Item[%d] id=%s Value = %f , Weight = %d, Volume = %d\n",
            //        i, items[i].id, items[i].value, items[i].weight, items[i].volume);

            w -= items[i].weight;
            v -= items[i].volume;
            i--;
        }
        else
            i--;
    }
    return knapsack;
}

void print_matrix(Matrix_t *matrix)
{
    /* PRINTING MATRIX */

    printf("\n---------------------------------------\n");
    printf("%*s  ", 3, "I");
    for (int w = 0; w < matrix->cols; w++)
        printf(" W%*d   ", 2, w);
    printf("\n");
    for (int v = 0; v < matrix->depth; v++)
    {
        for (int i = 0; i < matrix->rows; i++)
        {
            printf("%*d ", 3, i);
            for (int w = 0; w < matrix->cols; w++)
            {
                printf("%*.2f ", 6, get_value(matrix, i, w, v));
            }
            printf("\n");
        }
        printf("---------------------------------------\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        perror("Quantidade de argumentos insuficientes!");

    input_fp = fopen(argv[1], "r");
    output_fp = fopen(argv[2], "w");

    if (input_fp == NULL)
        printf("Nao foi possivel abrir o arquivo!");

    // Vehicles
    fscanf(input_fp, "%d", &N);

    Vehicle_t *vehicles = (Vehicle_t *)malloc(sizeof(Vehicle_t) * N);

    int32_t maximum_vol = 0, maximum_capw = 0;
    int32_t minimum_vol = INT32_MAX, minimum_capw = INT32_MAX;

    for (int i = 0; i < N; i++)
    {
        fscanf(input_fp, "%s %d %d", vehicles[i].plate, &vehicles[i].weight_cap, &vehicles[i].volume_cap);
        if (vehicles[i].volume_cap > maximum_vol)
            maximum_vol = vehicles[i].volume_cap;
        if (vehicles[i].volume_cap < minimum_vol)
            minimum_vol = vehicles[i].volume_cap;

        if (vehicles[i].weight_cap > maximum_capw)
            maximum_capw = vehicles[i].weight_cap;
        if (vehicles[i].volume_cap < minimum_capw)
            minimum_capw = vehicles[i].volume_cap;

    }

    // Items
    fscanf(input_fp, "%d", &M);
    loadItems(input_fp, &items, M);

    printf("----------------------------------------\n");

    /* CALCULATING MAIN MATRIX */

    for (int vh = 0; vh < N; vh++)
    {
        Matrix_t matrix = {
            .cols = vehicles[vh].weight_cap + 1,
            .depth = vehicles[vh].volume_cap + 1,
            .rows = M + 1,
            .arr_ptr = (double *)malloc(
                (vehicles[vh].weight_cap + 1) * (vehicles[vh].volume_cap + 1) * (M + 1) * sizeof(double)),
        };
        //printf("[Matrix] rows = %d, cols = %d, depth = %d\n\n", matrix.rows, matrix.cols, matrix.depth);

        calc_value_mt(&matrix, items, minimum_vol, minimum_capw);
        // print_matrix(&matrix);

        /* Items Selection */
        {
            Knapsack_t knapsack = select_items(&matrix, &vehicles[vh], items, M);

            fprintf(output_fp, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)\n",
                    vehicles[vh].plate,
                    knapsack.value,
                    knapsack.weight, (int)((double)(knapsack.weight / vehicles[vh].weight_cap) * 100.0),
                    knapsack.volume, (int)((double)(knapsack.volume / vehicles[vh].volume_cap) * 100.0));

            for (int i = knapsack.count - 1; i >= 0; i--)
                fprintf(output_fp, "%s\n", knapsack.items[i].id);
        }

        for (int i = 0; i < M; i++)
        {
            if (items[i].allocated)
            {
                for (int j = i; j < M - 1; j++)
                {
                    items[j] = items[j + 1];
                }
                M--;
                i--;
            }
        }
    }

    /* PENDING ITEMS */
    {
        fprintf(output_fp, "[PENDENTE]R$%.2f,%dKG,%dL:\n", pending_value, pending_weight, pending_volume);
        for (int p = 1; p < M; p++)
            fprintf(output_fp, "%s\n", items[p].id);
    }

    fclose(input_fp);
    fclose(output_fp);
}