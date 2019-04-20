
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
    for (int i = 0; i < r; ++i){
        rows[i] = values + i * c;
    }
    return rows;
}

void destroy2D(double** arr){
    free(*arr);
    free(arr);
}

//reference: https://www.geeksforgeeks.org/pass-2d-array-parameter-c/
//input: X
//output: reformatted double** X_ref
double** reformat(int rows, int cols, double X[][cols]){
    double** ref_matrix = create2D(rows, 2 * cols + 1);
    for(int i = 0; i < rows; i++){
        int idx = 0;
        for(int j = 0; j < cols; j++){
            if(X[i][j] != 0){
                ref_matrix[i][idx] = j;
                ref_matrix[i][idx + 1] = X[i][j];
                idx += 2;
            }
        }
        ref_matrix[i][idx] = -1;
    }
    return ref_matrix;
}

//https://www.cs.cmu.edu/~scandal/cacm/node9.html
//input: Xref, beta
//output: X @ beta
double* sparse_dot(int X_rows, double** Xref, double* beta){
    double* result = malloc(X_rows * sizeof(double));
    for(int i = 0; i < X_rows; i++){
        int j = 0;
        double sum = 0;
        while(Xref[i][j] != -1){
            int idx = Xref[i][j];
            sum += Xref[i][j + 1] * beta[idx];
            j += 2;
        }
        result[i] = sum;
    }
    return result;
}

//input: v1, v2
//output: v3 = v1 - v2
double* vec_sub(int len, double* v1, double* v2){
    double* v3 = malloc(len * sizeof(double));
    for(int i = 0; i < len; i++){
        v3[i] = v1[i] - v2[i];
    }
    return v3;
}

//input: vector vec
//output: ||vec||^2
double L2(int len, double* vec){
    double sum = 0;
    for(int i = 0; i < len; i++){
        sum += vec[i] * vec[i];
    }
    return sqrt(sum);
}

//input: X_ref, beta, y
//output: f = ||XB - y||^2
double f_linear(int X_rows, double** X_ref, double* beta, double* y){
    double* XB = sparse_dot(X_rows, X_ref, beta);
    double* XB_y = vec_sub(X_rows, XB, y);
    double L2_XB_y = L2(X_rows, XB_y);
    
    return L2_XB_y;
}

int main(){
    // Given X, beta, and y
    // f = ||XB - y||^2
    // df = 2 * X.T * (XB - y)
    
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
    double** X_ref = reformat(rows, cols, X);
    print2D(X_ref, rows);
    
    double val = f_linear(rows, X_ref, beta, y);
    printf("val: %f\n", val);
    
    destroy2D(X_sparse);
    
    
    return 0;
}