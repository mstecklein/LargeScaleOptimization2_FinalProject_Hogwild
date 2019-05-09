/*
 * Problem encapsulates the current optimization problem being
 *   evaluated by the program.
 */

#ifndef _problem_h_
#define _problem_h_

#include <stdio.h>
#include "data.h"
#include "thread_array.h"


typedef struct _problem_t {
	int (*gradient)(thread_array_t iterate, sparse_array_t sparse_sample_x, double sample_y, sparse_array_t *ret_sample_grad);
	// Algorithm:
	int (*algo_update_func)(thread_array_t, data_t*, int);
	int (*algo_init_func)(int, int);
	int (*algo_deinit_func)(void);
	double stepsize;
	int num_total_iter;
	int num_log_points;
} problem_t;


extern problem_t current_problem;


static inline void set_current_problem(problem_t problem) {
	current_problem = problem;
}

static inline problem_t get_current_problem(void) {
	return current_problem;
}

static inline double get_stepsize(void) {
	return current_problem.stepsize;
}

// Returns the gradient for a sample given the current iterate,
//   the sampled data point, and its respective label.
static inline int gradient(thread_array_t iterate, sparse_array_t sparse_sample_x, double sample_y, sparse_array_t *ret_sample_grad) {
	return current_problem.gradient(iterate, sparse_sample_x, sample_y, ret_sample_grad);
}


#endif // _problem_h_
