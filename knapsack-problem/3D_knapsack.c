#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define CONSTRAINT_MULT 65521 // Largest 16bit Prime Number

typedef struct
{
    char *id[13];
    int value;
    int weight;
    int volume;
    int constraint;
} Item_t;

typedef struct
{
    char *plate[7];
    int weight_cap;
    int volume_cap;
} Vehicle_t;

typedef struct
{
    Item_t *items;
    int count;
} Knapsack_t;

typedef struct
{
    int rows;
    int cols;
    int depth;
    int32_t *arr_ptr;

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

void add_item(Knapsack_t *sack, Item_t item)
{
    sack->items[sack->count] = item;
    sack->count++;
}

int get_value(Matrix_t *matrix, int x, int y, int z)
{
    // access by V_i_w[i * W + j]
    int c = matrix->cols;
    int r = matrix->rows;

    return matrix->arr_ptr[z * r * c + x * c + y];
}

void set_value(Matrix_t *matrix, int x, int y, int z, int value)
{
    int c = matrix->cols;
    int r = matrix->rows;
    matrix->arr_ptr[z * c * r + x * c + y] = value;
}

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

void calc_value_mt(Matrix_t *matrix, Item_t *items)
{
    /* ----------- Width -----------*/
    // Face XY = 0
    for (int x = 0; x < matrix->rows; x++)
        for (int y = 0; y < matrix->cols; y++)
            set_value(matrix, x, y, 0, 0);

    // Face XZ = 0
    for (int x = 0; x < matrix->rows; x++)
        for (int z = 0; z < matrix->depth; z++)
            set_value(matrix, x, 0, z, 0);

    // Face YZ = 0
    for (int y = 0; y < matrix->cols; y++)
        for (int z = 0; z < matrix->depth; z++)
            set_value(matrix, 0, y, z, 0);

    for (int i = 1; i < matrix->rows; i++)          // i = X axis
        for (int w = 1; w < matrix->cols; w++)      // w = Y axis
            for (int v = 1; v < matrix->depth; v++) // v = Z axis
            {
                if (w < items[i - 1].weight && v < items[i - 1].volume) // Does not Fit
                {
                    set_value(
                        matrix,
                        i, w, v,                   // pos
                        get_value(matrix, i, w, v) // value
                    );
                }
                else // Fits
                {
                    // TODO: Verificar essa captura de valor máximo
                    int max_value =
                        max(
                            get_value(matrix, i - 1, w - items[i - 1].weight + items[i - 1].value, v - 1),
                            get_value(matrix, i - 1, w - 1, v - items[i - 1].volume + items[i - 1].value)
                        );
                    
                    // Setting the max Value
                    set_value(
                        matrix,
                        i, w, v,
                        max_value
                    );
                }
            }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        perror("Quantidade de argumentos insuficientes!");

    FILE *input_fp = fopen(argv[1], "r");

    if (input_fp == NULL)
        printf("Nao foi possivel abrir o arquivo!");
    int N, M;

    // Vehicles
    fscanf("%d", &N);

    Vehicle_t *vehicles = (Vehicle_t *)malloc(sizeof(Vehicle_t) * N);

    for (int i = 0; i < N; i++)
        fscanf("%c %d %d", &vehicles[i].plate, &vehicles[i].weight_cap, &vehicles[i].volume_cap);

    // Items
    fscanf("%d", &M);

    Item_t *items = (Item_t *)malloc(sizeof(Item_t) * M);

    for (int i = 0; i < M; i++)
    {
        fscanf("%c %f %d %d", &items[i].id, &items[i].value, &items[i].weight, &items[i].volume);
        items[i].constraint = (items[i].weight * CONSTRAINT_MULT) + items[i].volume;
    }

    for (int v = 0; v < N; v++)
    {
        Matrix_t matrix = {
            .cols = vehicles[v].weight_cap,
            .rows = N + 1,
            .arr_ptr = (int32_t *)malloc((vehicles[v].weight_cap + 1) * (N + 1) * sizeof(int32_t)),
        };

        calc_value_mt(&matrix, items);

        for (int i = 0; i < matrix.rows; i++)
        {
            for (int j = 0; j < matrix.cols; j++)
            {
                int x = get_value(&matrix, i, j);
                printf("%*d ", 3, x);
            }
            printf("\n");
        }
    }
}