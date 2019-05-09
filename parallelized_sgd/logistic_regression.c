#include <math.h>
#include "logistic_regression.h"


int logreg_gradient(thread_array_t iterate, sparse_array_t sparse_sample_x, double sample_y, sparse_array_t *ret_sample_grad) {
    // grad = X.T * (sigmoid(XB) - y))
	double XB = dot_sparse_dense(sparse_sample_x, iterate);
	double sig_XB_y = (1 / (1 + exp(-XB))) - sample_y;
	int XT_sig_XB_y = scale_sparse(ret_sample_grad, sparse_sample_x, sig_XB_y);
	return 0;
}
