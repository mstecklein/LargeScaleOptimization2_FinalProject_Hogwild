/*
 * The functions necessary to define the Linear Regression optimization
 *   problem.
 */

#ifndef _linear_regression_h_
#define _linear_regression_h_

#include "data.h"
#include "thread_array.h"
#include "linear_algebra.h"


// Returns the gradient for a sample given the current iterate,
//   the sampled data point, and its respective label.
int linreg_gradient(thread_array_t iterate, sparse_array_t sparse_sample_x, double sample_y, sparse_array_t *ret_sample_grad);


#endif // _linear_regression_h_
