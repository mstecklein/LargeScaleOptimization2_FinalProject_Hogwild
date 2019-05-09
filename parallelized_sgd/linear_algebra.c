#include "linear_algebra.h"


int add_dense(double *result, double *v1, double *v2, int len){
    for(int i = 0; i < len; i++){
        result[i] = v1[i] + v2[i];
    }
    return 0;
}


int scale_sparse(sparse_array_t *result, sparse_array_t sparse_v1, double scalar) {
	result->len = sparse_v1.len;
	for (int i = 0; i < sparse_v1.len; i++) {
		result->pts[i] = sparse_v1.pts[i];
		result->pts[i].value *= scalar;
	}
	return 0;
}


double dot_sparse_dense(sparse_array_t sparse_v1, thread_array_t dense_v2){
	double sum = 0;
	for (int i = 0; i < sparse_v1.len; i++) {
		sum += sparse_v1.pts[i].value * TA_idx(dense_v2, sparse_v1.pts[i].index, double);
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
