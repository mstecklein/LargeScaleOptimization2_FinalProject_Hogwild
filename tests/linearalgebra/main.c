#include <stdio.h>
#include <math.h>

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
//output: v3 = v1 + v2
double* vec_add(int len, double* v1, double* v2){
    double* v3 = malloc(len * sizeof(double));
    for(int i = 0; i < len; i++){
        v3[i] = v1[i] + v2[i];
    }
    return v3;
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
    
    free(XB);
    free(XB_y);
    
    return L2_XB_y;
}

//input: X
//output: X.T
double** transpose(int rows, int cols, double** X){
    double** trans = create2D(cols, rows);
    for(int i = 0; i < cols; i++){
        for(int j = 0; j < rows; j++){
            trans[i][j] = X[j][i];
        }
    }
    return trans;
}

//input: X_ref, X_T_ref, beta, y
//output: grad f
double* gradf_linear(int rows, int cols, double** X_ref, double** X_T_ref, double* beta, double* y){
    double* XB = sparse_dot(rows, X_ref, beta);
    double* XB_y = vec_sub(rows, XB, y);
    double* X_T_XB_y = sparse_dot(cols, X_T_ref, XB_y);
    double* X_T_XB_y_2 = vec_scale(rows, X_T_XB_y, 2);
    
    free(XB);
    free(XB_y);
    free(X_T_XB_y);
    
    return X_T_XB_y_2;
}

int main(){
    printf("Ignore the extra numbers at the end of each row for print1D and print2D!\n");
 
    // init X, beta, y
    
    ///////// MODIFY THIS //////////
    double n = 0.01; //step-size
    int rows = 5;
    int cols = 4;
    double X_data[20] = {0,1,0,0,
                        3,1,0,-2,
                        0,4,0,0,
                        0,0,1,1,
                        -1,0,-3,0};
    double beta_data[4] = {1,2,3,4};
    double y[5] = {-2,-1,0,1,2};
    
    /*
    double n = 0.0001; //step-size
    int rows = 20;
    int cols = 1;
    double X_data[20] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    double beta_data[1] = {10};
    double y[20] = {.5,1,1.5,3,3.5,5.1,6.3,6.7,8.1,9,10,11.4,11.9,12.5,13.9,14.8,16.2,17.3,18,19.1};*/
    ////////////////////////////////
    
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
    
    
    // get X.T
    double** X_T = transpose(rows, cols, X);
    
    // reformat X and X.T
    double** X_ref = reformat(rows, cols, X);
    double** X_T_ref = reformat(cols, rows, X_T);
    
    // update beta <- beta - n * df_val
    for(int i = 0; i < 20; i++){
        double f_val = f_linear(rows, X_ref, beta, y);
        double* df_val = gradf_linear(rows, cols, X_ref, X_T_ref, beta, y);
        
        double* n_df = vec_scale(cols, df_val, n);
        double* beta1 = vec_sub(cols, beta, n_df);
        
        free(df_val);
        free(n_df);
        free(beta);
        
        beta = beta1;
        
        printf("f: %f, beta: ", f_val);
        print1D(beta, cols);
    }
    
    // free memory
    free(beta);
    
    destroy2D(X);
    destroy2D(X_T);
    destroy2D(X_ref);
    destroy2D(X_T_ref);
    
    return 0;
}