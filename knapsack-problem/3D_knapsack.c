#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    char id[13];
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

void add_item(Knapsack_t *sack, Item_t item)
{
    sack->items[sack->count] = item;
    sack->count++;

    sack->value  += item.value;
    sack->weight += item.weight;
    sack->volume += item.volume;
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

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

void calc_value_mt(Matrix_t *matrix, Item_t *items)
{
    for (int v = 1; v < matrix->depth; v++)        // v = Z axis
        for (int i = 1; i < matrix->rows; i++)     // i = X axis
            for (int w = 1; w < matrix->cols; w++) // w = Y axis
            {
                if (w < items[i - 1].weight || v < items[i - 1].volume) // dont fit
                {
                    int value = get_value(matrix, i - 1, w, v);
                    set_value(matrix, i, w, v, value);
                }
                else // fits
                {
                    // printf("[Comparation] %d < %d && %d < %d  ", w, items[i - 1].weight, v, items[i - 1].volume);
                    int value_with_item = get_value(matrix, i - 1, w - items[i - 1].weight, v) + items[i - 1].value;
                    int value_without_item = get_value(matrix, i - 1, w, v);

                    // printf("[Calculation] (%d, %d, %d): (i-1, w) = %d\t(i-1, w-wi)+vi = %d\n", i, w, v, value_without_item, value_with_item);

                    set_value(matrix, i, w, v, max(value_with_item, value_without_item));
                }
            }
}

Knapsack_t select_items(Matrix_t *matrix, Vehicle_t *vehicle, Item_t *items, int32_t items_count)
{
    int v = vehicle->volume_cap;
    int i = items_count;
    int w = vehicle->weight_cap;

    Knapsack_t knapsack = {
        .value = 0,
        .volume = 0,
        .count = 0,
        .weight = 0,
        .items = (Item_t *)malloc(sizeof(Item_t) * 255) // Arbitrary size
    };

    while (v > 0 && i > 0 && w > 0)
    {
        if (get_value(matrix, i, w, v) != get_value(matrix, i - 1, w, v) && items[i - 1].allocated == false)
        {
            items[i - 1].allocated = true;

            add_item(&knapsack, items[i-1]);

            i -= 1;
            w -= items[i].weight;
            v -= items[i].volume;
        }
        else
        {
            i -= 1;
        }
    }

    return knapsack;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        perror("Quantidade de argumentos insuficientes!");

    input_fp = fopen(argv[1], "r");
    output_fp = fopen(argv[2], "w");

    if (input_fp == NULL)
        printf("Nao foi possivel abrir o arquivo!");

    int N, M;

    // Vehicles
    fscanf(input_fp, "%d", &N);

    Vehicle_t *vehicles = (Vehicle_t *)malloc(sizeof(Vehicle_t) * N);

    int32_t maximum_vol = 0, maximum_capw = 0;

    for (int i = 0; i < N; i++)
    {
        fscanf(input_fp, "%s %d %d", vehicles[i].plate, &vehicles[i].weight_cap, &vehicles[i].volume_cap);
        if (vehicles[i].volume_cap > maximum_vol)
            maximum_vol = vehicles[i].volume_cap;

        if (vehicles[i].weight_cap > maximum_capw)
            maximum_capw = vehicles[i].weight_cap;
    }

    // Items
    fscanf(input_fp, "%d", &M);

    Item_t *items = (Item_t *)malloc(sizeof(Item_t) * M);

    for (int i = 0; i < M; i++)
    {
        fscanf(input_fp, "%13s %le %d %d", items[i].id, &items[i].value, &items[i].weight, &items[i].volume);
        items[i].allocated = false;

        // printf("%13s %f %d %d\n", items[i].id, items[i].value, items[i].weight, items[i].volume);
    }

    Matrix_t matrix = {
        .cols = maximum_capw + 1,
        .depth = maximum_vol + 1,
        .rows = M + 1,
        .arr_ptr = (double *)malloc(
            (maximum_capw + 1) * (maximum_vol + 1) * (M + 1) * sizeof(double)),
    };

    printf("[Matrix] rows = %d, cols = %d, depth = %d\n", matrix.rows, matrix.cols, matrix.depth);

    calc_value_mt(&matrix, items);

    for (int vh = 0; vh < N; vh++)
    {
        Knapsack_t knapsack = select_items(&matrix, &vehicles[vh], items, M);

        fprintf(output_fp, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)\n",
                vehicles[vh].plate,
                knapsack.value,
                knapsack.weight, ((vehicles[vh].weight_cap - knapsack.weight) / vehicles[vh].weight_cap) * 100,
                knapsack.volume, ((vehicles[vh].volume_cap - knapsack.volume) / vehicles[vh].volume_cap) * 100);

        for (int i = 0; i < knapsack.count; i++)
            fprintf(output_fp, "%s\n", knapsack.items[i].id);
    }

    // Pending Items
    Knapsack_t pending_sack = {
        .value  = 0.0,
        .weight = 0,
        .volume = 0,
        .count  = 0,
        .items  = (Item_t *) malloc(sizeof (Item_t) * M) // Arbitray atribution (max = none item allocated)
    }; 

    for (int i = 0; i < M; i++)
        if(!items[i].allocated)
            add_item(&pending_sack, items[i]);

    fprintf(output_fp, "[PENDENTE]R$%.2f,%dKG,%dL\n", pending_sack.value, pending_sack.weight,pending_sack.volume);
    
    for (int i = 0; i < pending_sack.count; i++)
        fprintf(output_fp, "%s\n", pending_sack.items[i].id);

    fclose(input_fp);
    fclose(output_fp);

}