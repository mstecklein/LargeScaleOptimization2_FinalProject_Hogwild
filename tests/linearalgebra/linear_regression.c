#include "linear_regression.h"

int linreg_gradient(double *iterate, double* sample_x, sparse_point_t *sparse_sample_x, double sample_y, double *ret_sample_grad, int num_features) {
    // grad = 2 * X.T * (XB - y)
    
    double XB_y = dot_sparse_dense(sparse_sample_x, iterate) - sample_y;
    int XT_XB_y_2 = scale(ret_sample_grad, sample_x, 2 * XB_y, num_features);
    
	return 0;
}
