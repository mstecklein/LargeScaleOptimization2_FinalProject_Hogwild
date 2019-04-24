/*
 * The functions necessary for linear algebra operations.
 */

#ifndef _linear_algebra_h_
#define _linear_algebra_h_

#include "data.h"

// Returns result = v1 + v2
int add_dense(double *result, double *v1, double *v2, int len);

// Returns result = v1 * scalar
int scale(double *result, double *v1, double scalar, int len);

// Return sparse_v1 * dense_v2
double dot_sparse_dense(sparse_point_t* sparse_v1, double* dense_v2);

//double dot_dense_dense(double *dense_v1, double *dense_v2, int len);

//double dot_sparse_sparse(sparse_point_t *sparse_v1, sparse_point_t *sparse_v2);

#endif // _linear_algebra_h_
