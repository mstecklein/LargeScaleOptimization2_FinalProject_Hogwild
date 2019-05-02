/*
 * The functions necessary for linear algebra operations.
 */

#ifndef _linear_algebra_h_
#define _linear_algebra_h_

#include "data.h"
#include "thread_array.h"

// Returns result = v1 + v2
int add_dense(double *result, double *v1, double *v2, int len);

// Returns result = sparse_v1 * scalar
int scale_sparse(sparse_array_t *result, sparse_array_t sparse_v1, double scalar);

// Return sparse_v1 * dense_v2
double dot_sparse_dense(sparse_array_t  sparse_v1, thread_array_t dense_v2);

// Return dense_v1 * dense_v2
double dot_dense_dense(double *dense_v1, double *dense_v2, int len);

#endif // _linear_algebra_h_
