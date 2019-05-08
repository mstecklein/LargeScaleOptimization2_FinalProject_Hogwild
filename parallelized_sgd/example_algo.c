#include "example_algo.h"



#define ITERATE(i)	TA_idx(iterate, i, double)
#define RAND_SEED	&TA_idx(rng_seedp, thread_num, unsigned int)
static thread_array_t /*unsigned int*/ rng_seedp; // seed state for random number generator, one for each thread



int example_algo_update_independent(thread_array_t iterate, data_t *data, int thread_num) {
	// Select a deterministic index to update. This index will never be
	//   selected by other threads.
	int index = thread_num; // XXX assumes number of features is > number of threads

	// Arbitrary increment of the iterate at the selected index
	ITERATE(index) += 0.1;

	return 0;
}


int example_algo_update_shared(thread_array_t iterate, data_t *data, int thread_num) {
	// Select a random index to update. This index may also be selected by
	//   other threads, either in the past present or future.
	int index = rand_r(RAND_SEED) % data->num_samples;

	// Arbitrary increment of the iterate at the selected index
	ITERATE(index) += 0.1;

	return 0;
}



int example_algo_initialize(int num_features, int num_threads) {
	// Initialize RNG seed states
	malloc_thread_array(&rng_seedp, num_threads);
	for (int n = 0; n < num_threads; n++) {
		TA_idx(rng_seedp, n, unsigned int) = rand() + n;
	}

	return 0;
}


int example_algo_deinitialize(void) {
	// Free RNG seed states
	free_thread_array(&rng_seedp);

	return 0;
}
