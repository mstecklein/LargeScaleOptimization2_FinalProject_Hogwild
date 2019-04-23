#include "problem.h"


problem_t current_problem;


void set_current_problem(problem_t problem) {
	current_problem = problem;
}

problem_t get_current_problem(void) {
	return current_problem;
}

double get_stepsize(void) {
	return current_problem.stepsize;
}

int gradient(double *iterate, double *sample_x, double sample_y, double *sample_grad) {
	return current_problem.gradient(iterate, sample_x, sample_y, sample_grad);
}
