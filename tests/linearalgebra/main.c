#include <stdio.h>
#include <math.h>

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
double** reformat(int rows, int cols, double** X){
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

//input: v1, scalar
//output: v1 * scalar
double* vec_scale(int len, double* v1, double scalar){
    double* v3 = malloc(len * sizeof(double));
    for(int i = 0; i < len; i++){
        v3[i] = scalar * v1[i];
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

//input: X
//output: X.T
double** transpose(int rows, int cols, double** X){
    double** trans = create2D(rows, cols);
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            trans[i][j] = X[j][i];
        }
    }
    return trans;
}

//input: X_ref, X_T_ref, beta, y
//output: grad f
double* gradf_linear(int X_rows, double** X_ref, double** X_T_ref, double* beta, double* y){
    double* XB = sparse_dot(X_rows, X_ref, beta);
    double* XB_y = vec_sub(X_rows, XB, y);
    double* X_T_XB_y = sparse_dot(X_rows, X_T_ref, XB_y);
    
    return vec_scale(X_rows, X_T_XB_y, 2);
}

int main(){
    printf("Ignore the extra numbers at the end of each row for print1D and print2D!\n");
 
    // init X, beta, y
    
    ///////// MODIFY THIS //////////
    int rows = 3;
    int cols = 3;
    double X_data[9] = {0,1,0,
                        3,1,0,
                        0,4,0};
    double beta[3] = {1,2,3};
    double y[3] = {-1,0,1};
    ////////////////////////////////
    
    double** X = create2D(rows, cols);
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            X[i][j] = X_data[i * rows + j];
        }
    }
    
    // get X.T
    double** X_T = transpose(rows, cols, X);
    
    // reformat X and X.T
    double** X_ref = reformat(rows, cols, X);
    double** X_T_ref = reformat(rows, cols, X_T);
    
    // calculate f and df
    double f_val = f_linear(rows, X_ref, beta, y);
    double* df_val = gradf_linear(rows, X_ref, X_T_ref, beta, y);
    printf("f: %f\ndf: ", f_val);
    print1D(df_val);
    
    // free memory
    destroy2D(X);
    destroy2D(X_T);
    destroy2D(X_ref);
    destroy2D(X_T_ref);
    
    return 0;
}