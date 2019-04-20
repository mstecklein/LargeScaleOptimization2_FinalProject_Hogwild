
#include <stdio.h>
#include <math.h>

/*typedef struct matrix_struct {
    int r;
    int c;
    double** data;
} matrix;*/

void print1D(double* v){
    for(int i = 0; i < 12; i++){
        printf("%f ", v[i]);
    }
    printf("\n");
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

//https://stackoverflow.com/questions/5201708/how-to-return-a-2d-array-to-a-function-in-c
double** create2D(int r, int c){
    double* values = calloc(r * c, sizeof(double));
    double** rows = malloc(r * sizeof(double*));
    for (int i = 0; i < r; ++i)
    {
        rows[i] = values + i * c;
    }
    return rows;
}

void destroy2D(double** arr){
    free(*arr);
    free(arr);
}

//https://www.geeksforgeeks.org/pass-2d-array-parameter-c/
double** make_sparse_format(int rows, int cols, double X[][cols]){
    double** sparse = create2D(rows, 2 * cols + 1);
    for(int i = 0; i < rows; i++){
        int idx = 0;
        for(int j = 0; j < cols; j++){
            if(X[i][j] != 0){
                sparse[i][idx] = j;
                sparse[i][idx + 1] = X[i][j];
                idx += 2;
            }
        }
        sparse[i][idx] = -1;
    }
    return sparse;
}

//https://www.cs.cmu.edu/~scandal/cacm/node9.html
double* sparse_dot(int X_rows, double** Xsp, double* beta){
    double* result = malloc(X_rows * sizeof(double));
    for(int i = 0; i < X_rows; i++){
        int j = 0;
        double sum = 0;
        while(Xsp[i][j] != -1){
            int idx = Xsp[i][j];
            sum += Xsp[i][j + 1] * beta[idx];
            j += 2;
        }
        result[i] = sum;
    }
    return result;
}

double* vec_sub(int len, double* v1, double* v2){
    double* v3 = malloc(len * sizeof(double));
    for(int i = 0; i < len; i++){
        v3[i] = v1[i] - v2[i];
    }
    return v3;
}

double L2(int len, double* vec){
    double sum = 0;
    for(int i = 0; i < len; i++){
        sum += vec[i] * vec[i];
    }
    return sqrt(sum);
}

double f_linear(int X_rows, double** X_sp, double* beta, double* y){
    double* XB = sparse_dot(X_rows, X_sp, beta);
    double* XB_y = vec_sub(X_rows, XB, y);
    double L2_XB_y = L2(X_rows, XB_y);
    
    return L2_XB_y;
}

int main(){
    // Given X, beta, and y. Want to perform gradient updates for linear regression.
    // f = ||XB - y||^2
    // df = 2 * X.T * (XB - y)
    
    // Givin rows, cols, and double**
    
    // Say X = 
    //  2 -1  0  0
    // -1  2 -1  0
    //  0 -1  2 -1
    //  0  0 -1  2
    
    // Say beta = 
    //  1
    //  0
    // -1
    //  0
    
    double X[3][3] = {0,1,0,3,1,0,0,4,0};
    double beta[3] = {1,2,3};
    double y[3] = {-1,0,1};
    int rows = 3;
    int cols = 3;
    
    // currently, X must be a 2d array, not double**.  Not sure if it makes a difference
    double** X_sparse = make_sparse_format(rows, cols, X);
    print2D(X_sparse, rows);
    
    double val = f_linear(rows, X_sparse, beta, y);
    printf("val: %f\n", val);
    
    destroy2D(X_sparse);
    
    
    return 0;
}