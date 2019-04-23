/*
 * The functions necessary to define the Linear Regression optimization
 *   problem.
 */

#ifndef _linear_regression_h_
#define _linear_regression_h_


// Returns the gradient for a sample given the current iterate,
//   the sampled data point, and its respective label.
int linreg_gradient(double *iterate, double *sample_x, double sample_y, double *sample_grad);


#endif // _linear_regression_h_
