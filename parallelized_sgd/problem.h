/*
 * Problem encapsulates the current optimization problem being
 *   evaluated by the program.
 */

#ifndef _problem_h_
#define _problem_h_

#include "data.h"


typedef struct _problem_t {
	int (*gradient)(double *iterate, sparse_point_t *sparse_sample_x, double sample_y, double *ret_sample_grad, double *scratchpad);
	// Algorithm:
	int (*algo_update_func)(double*, data_t*, int);
	int (*algo_init_func)(int, int);
	int (*algo_deinit_func)(void);
	double stepsize;
} problem_t;


extern problem_t current_problem;


void set_current_problem(problem_t problem);

problem_t get_current_problem(void);

double get_stepsize(void);

// Returns the gradient for a sample given the current iterate,
//   the sampled data point, and its respective label.
int gradient(double *iterate, sparse_point_t *sparse_sample_x, double sample_y, double *ret_sample_grad, double *scratchpad);


#endif // _problem_h_
