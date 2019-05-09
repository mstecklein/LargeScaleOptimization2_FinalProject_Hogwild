#include<pthread.h>
#include "data.h"
#include "problem.h"
#include "naive_psgd.h"


static sparse_array_t sparse_sample_grad;
static pthread_mutex_t lock;

static thread_array_t /*unsigned int*/ rng_seedp; // seed state for random number generator, one for each thread


int naive_psgd(thread_array_t iterate, data_t *data, int thread_num) {
	// Get random sample
	int rand_index = rand_r(&TA_idx(rng_seedp, thread_num, unsigned int)) % data->num_samples;
	sparse_array_t sparse_sample_X = data->sparse_X[rand_index];
	double sample_y = data->y[rand_index];

	// Lock
	pthread_mutex_lock(&lock);

	// Evaluate gradient
	gradient(iterate, sparse_sample_X, sample_y, &sparse_sample_grad);
	// Update coordinate individually and atomically
	for (int i = 0; i < sparse_sample_grad.len; i++) {
		int index    = sparse_sample_grad.pts[i].index;
		double value = sparse_sample_grad.pts[i].value;
		TA_idx(iterate, index, double) -= get_stepsize()*value;
	}

	// Unlock
	pthread_mutex_unlock(&lock);

	return 0;
}



int naive_psgd_initialize(int num_features, int num_threads) {
	sparse_sample_grad.len = 0;
	sparse_sample_grad.pts = (sparse_point_t *) malloc(num_features * sizeof(sparse_point_t));
	malloc_thread_array(&rng_seedp, num_threads);
	for (int n = 0; n < num_threads; n++) {
		TA_idx(rng_seedp, n, unsigned int) = rand() + n;
	}
	return pthread_mutex_init(&lock, NULL);
}


int naive_psgd_deinitialize(void) {
	free(sparse_sample_grad.pts);
	pthread_mutex_destroy(&lock);
	free_thread_array(&rng_seedp);
	return 0;
}
