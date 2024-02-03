#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct{
    int value;
    int weight;
} Item;

typedef struct{
    int rows;
    int cols;
    int* arr_ptr;
} Matrix_t;

int get_value(Matrix_t* matrix, int i , int j){
    // access by V_i_w[i * W + j]
    int c = matrix->cols;

    return matrix->arr_ptr[i * c + j];
}

void set_value(Matrix_t* matrix,int i , int j, int value){
    int c = matrix->cols;
    matrix->arr_ptr[i * c + j] = value;
}

int max(int a, int b)
{
    if(a > b)
        return a;
    return b;
}

int main(int argc, char* argv[])
{
    if(argc < 3)
        perror("Quantidade de argumentos insuficientes!");

    FILE *input_fp ;
    uint32_t N, C;

    if(input_fp = fopen(argv[1], "r") == NULL)
        printf("Nao foi possivel abrir o arquivo!");

    fscanf(input_fp, "%d %d", &N, &C);

    // Items
    Item* items = (Item*)malloc(sizeof(Item) * N);

    // setting Item`s values up
    for(int i = 0; i < N; i++)
        fscanf(input_fp, "%d", &items[i].value);

    // setting Item`s weight up
    for(int i = 0; i < N; i++)
        fscanf(input_fp, "%d", &items[i].weight);

    // N = rows (i); W = cols (j)
    Matrix_t* V_i_w = (Matrix_t *) malloc(N * C * sizeof(Matrix_t));

    V_i_w->cols = C;
    V_i_w->rows = N;

    // setting the first row
    for(int j = 0; j < C; j++)
        set_value(V_i_w, 0, j, 0);

    // setting the first col
    for(int i = 0; i < N; i++)
        set_value(V_i_w, i, 0, 0);


    // where N is the number of items
    for(int i = 1; i < N; i++){
        // where W is the max capacity of the sack
        for(int j = 1; i < C; j++){
            if(j - items[i].weight < 0){
                int x = get_value(V_i_w, i-1, j);
                set_value(V_i_w, i, j, x);
            }
            else
            { // Fits
                int32_t esq = get_value(V_i_w, i-1, j-1);
                int32_t dir = get_value(V_i_w, i-1, j) + items[i - 1].weight;

                int x = max(esq,dir);
                set_value(V_i_w, i, j, x);
            }

        }
    }



}
