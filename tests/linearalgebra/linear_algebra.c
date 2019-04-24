#include "linear_algebra.h"

int add_dense(double *result, double *v1, double *v2, int len){
    for(int i = 0; i < len; i++){
        result[i] = v1[i] + v2[i];
    }
    return 0;
}

int scale(double *result, double *v1, double scalar, int len){
    for(int i = 0; i < len; i++){
        result[i] = v1[i] * scalar;
    }
    return 0;
}

double dot_sparse_dense(sparse_point_t* sparse_v1, double* dense_v2){
    int i = 0;
    double sum = 0;
    int idx = sparse_v1[i].index;
    while(idx != -1){
        sum += sparse_v1[i].value * dense_v2[idx];
        i++;
        idx = sparse_v1[i].index;
    }
    return sum;
}

double dot_dense_dense(double *dense_v1, double *dense_v2, int len){
    double sum = 0;
    for(int i = 0; i < len; i++){
        sum += dense_v1[i] * dense_v2[i];
    }
    return sum;
}

/*double dot_sparse_sparse(sparse_point_t *sparse_v1, sparse_point_t *sparse_v2){
    int i = 0, j = 0;
    int idx = sparse_v1[i]->index;
    int jdx = sparse_v2[j]->index;
    
    double sum = 0;
    
    while(idx != -1 && jdx != -1){
        while(idx != jdx){
            if(idx < jdx){
                i++;
                idx = sparse_v1[i]->index;
            }
            else if(jdx < idx){
                j++;
                jdx = sparse_v2[j]->index;
            }
        }
        sum += sparse_v1[i]->value * sparse_v2[j]->value;
        i++;
        j++;
        idx = sparse_v1[i]->index;
        jdx = sparse_v2[j]->index;
    }
    return sum;
}*/