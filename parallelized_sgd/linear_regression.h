/*
 * The functions necessary to define the Linear Regression optimization
 *   problem.
 */

#ifndef _linear_regression_h_
#define _linear_regression_h_

#include "data.h"


// Returns the gradient for a sample given the current iterate,
//   the sampled data point, and its respective label.
int linreg_gradient(double *iterate, sparse_point_t *sparse_sample_x, double sample_y, double *ret_sample_grad, double *scratchpad);


#endif // _linear_regression_h_
