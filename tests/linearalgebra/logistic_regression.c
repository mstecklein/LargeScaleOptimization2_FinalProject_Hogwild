#include "logistic_regression.h"

int logreg_gradient(double *iterate, double* sample_x, sparse_point_t *sparse_sample_x, double sample_y, double *ret_sample_grad, int num_features) {
    // grad = X.T * (sigmoid(XB) - y))
    
    double XB = dot_sparse_dense(sparse_sample_x, iterate);
    double sig_XB_y = (1 / (1 + exp(-XB))) - sample_y;
    int XT_sig_XB_y = scale(ret_sample_grad, sample_x, sig_XB_y, num_features);
    
	return 0;
}
