#include <stdio.h>
#include "problem.h"


problem_t current_problem;


void set_current_problem(problem_t problem) {
	current_problem = problem;
}

problem_t get_current_problem(void) {
	return current_problem;
}

double get_stepsize(void) {
	if (current_problem.stepsize == 0.0)
		printf("WARNING: Step size is 0.0\n");
	return current_problem.stepsize;
}

int gradient(double *iterate, sparse_array_t sparse_sample_x, double sample_y, sparse_array_t *ret_sample_grad, double *scratchpad) {
	return current_problem.gradient(iterate, sparse_sample_x, sample_y, ret_sample_grad, scratchpad);
}
