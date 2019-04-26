#include <time.h>
#include <stdlib.h>
#include "data.h"
#include "problem.h"
#include "hogwild.h"


static double *sample_grad; // TODO need one of these for each thread
static double **scratchpad; // TODO add code to implement this


static void atomic_decrement(double *dest, double dec_amt) {
	double ret_val = 0.0;
	double orig_val = 1.0;
	double dec_val;
	// Perform a compare-and-swap. If the update was of dest was
	//   atomic, the returned value should match the orig_val's
	//   read of the destination.
	while (ret_val != orig_val) {
		orig_val = *dest;
		dec_val = orig_val - dec_amt;
		__atomic_exchange(dest, &dec_val, &ret_val, __ATOMIC_RELAXED);
	}
}


int hogwild(double *iterate, data_t *data, int thread_num) {
	// Get random sample
	int rand_index = rand() % data->num_samples;
	sparse_point_t *sparse_sample_X = data->sparse_X[rand_index];
	double sample_y = data->y[rand_index];

	// Read iterate
	// TODO is it necessary to copy the gradient value here?

	// Evaluate gradient
	printf("HOGWILD eval grad\n"); // TODO remove me
	gradient(iterate, sparse_sample_X, sample_y, sample_grad, scratchpad[thread_num]);

	// Update coordinate individually and atomically
	printf("HOGWILD atomic dec\n"); // TODO remove me
	for (int i = 0; i < data->num_features; i++) { // TODO modify this to only update on the support of the sample
		atomic_decrement(&iterate[i], get_stepsize()*sample_grad[i]);
	}

	return 0;
}


int hogwild_initialize(int num_features, int num_threads) {
	srand(time(NULL));
	sample_grad = (double *) malloc(num_features * sizeof(double));
	return 0;
}


int hogwild_deinitialize(void) {
	free(sample_grad);
	return 0;
}
