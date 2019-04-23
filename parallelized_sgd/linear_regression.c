#include "linear_regression.h"

int linreg_gradient(double *iterate, sparse_point_t *sparse_sample_x, double sample_y, double *ret_sample_grad, double *scratchpad) {
    int num_features = 0; // include this as a parameter?
    double XB_y = dot_sparse_dense(sparse_sample_x, iterate) - y;
    int XT_XB_y_2 = scale(ret_sample_grad, sparse_sample_x, 2 * XB_y, num_features);
    
	return 0;
}
