#include <stdio.h>
#include "linear_regression.h"
#include "linear_algebra.h"

void print1D(double* v, int len){
    printf("[");
    for(int i = 0; i < len - 1; i++){
        printf("%f, ", v[i]);
    }
    printf("%f]\n", v[len - 1]);
}

void print2D(double** sp, int rows){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < 12; j++){
            printf("%f ", sp[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printSparse(sparse_point_t** xsp, int rows){
    for(int i = 0; i < rows; i++){
        int j = 0;
        while(xsp[i][j].index != -1){
            printf("(%d, %f) ", xsp[i][j].index, xsp[i][j].value);
            j++;
        }
        printf("\n");
    }
}

//https://stackoverflow.com/questions/5201708/how-to-return-a-2d-array-to-a-function-in-c
double** create2D(int r, int c){
    double* values = calloc(r * c, sizeof(double));
    double** rows = malloc(r * sizeof(double*));
    for (int i = 0; i < r; ++i){
        rows[i] = values + i * c;
    }
    return rows;
}

void destroy2D(double** arr){
    free(*arr);
    free(arr);
}

int main(){
    ///////// MODIFY THIS ///////////////////////////////////////////////////////////////////////////
    /*double n = 0.01; //step-size
    int rows = 5;
    int cols = 4;
    double X_data[20] = {0,1,0,0,
                        3,1,0,-2,
                        0,4,0,0,
                        0,0,1,1,
                        -1,0,-3,0};
    double beta_data[4] = {1,2,3,4};
    double y[5] = {-2,-1,0,1,2};*/
    
    
    double n = 0.0001; //step-size
    int rows = 20;
    int cols = 1;
    double X_data[20] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    double beta_data[1] = {10};
    double y[20] = {.5,1,1.5,3,3.5,5.1,6.3,6.7,8.1,9,10,11.4,11.9,12.5,13.9,14.8,16.2,17.3,18,19.1};
    /////////////////////////////////////////////////////////////////////////////////////////////////
    
    double** X = create2D(rows, cols);
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            X[i][j] = X_data[i * cols + j];
        }
    }
    double* beta = malloc(cols * sizeof(double));
    for(int i = 0; i < cols; i++){
        beta[i] = beta_data[i];
    }
    
    // reformat X into sparse format
    sparse_point_t* values = calloc(rows * (cols + 1), sizeof(sparse_point_t));
    sparse_point_t** X_sparse = malloc(rows * sizeof(sparse_point_t*));
    for (int i = 0; i < rows; ++i){
        X_sparse[i] = values + i * (cols + 1);
    }
    
    for(int i = 0; i < rows; i++){
        int idx = 0;
        for(int j = 0; j < cols; j++){
            if(X[i][j] != 0){
                X_sparse[i][idx].index = j;
                X_sparse[i][idx].value = X[i][j];
                idx += 1;
            }
        }
        X_sparse[i][idx].index = -1;
    }
    
    double* scratch1 = malloc(cols * sizeof(double));
    double* scratch2 = malloc(cols * sizeof(double));
    
    for(int ep = 0; ep < 100; ep++){
        for(int i = 0; i < rows; i++){
            int linreg_grad = linreg_gradient(beta, X[i], X_sparse[i], y[i], scratch1, cols); // s1 = grad
            int scaled = scale(scratch2, scratch1, -n, cols); // s2 = -n * grad
            int added = add_dense(beta, beta, scratch2, cols);
        }
        print1D(beta, cols);
    }
    
    free(scratch1);
    free(scratch2);
    destroy2D(X);
    free(values);
    free(*X_sparse);
    free(X_sparse);
    free(beta);
    
    return 0;
}