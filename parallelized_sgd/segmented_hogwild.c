#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "data.h"
#include "problem.h"
#include "thread_array.h"
#include "psgd_analysis.h"
#include "segmented_hogwild.h"


static thread_array_t /*sparse_array_t*/ sparse_sample_grads;
static int num_threads;
static thread_array_t /*unsigned int*/ rng_seedp; // seed state for random number generator, one for each thread


#define THREAD_COUNTER	TA_idx(thread_counters, thread_num, unsigned int)
static thread_array_t /*unsigned int*/ thread_counters;
#define LOCAL_ITERATE TA_idx(local_iterates, thread_num, thread_array_t)
static thread_array_t /*thread_array_t*/ local_iterates;


int segmented_hogwild(thread_array_t iterate, data_t *data, int thread_num) {
	// Get random sample
	//    rand_r is reentrant (thread safe)
	int rand_index = rand_r(&TA_idx(rng_seedp, thread_num, unsigned int)) % data->num_samples;
	sparse_array_t sparse_sample_X = data->sparse_X[rand_index];
	double sample_y = data->y[rand_index];

	// Evaluate gradient
	sparse_array_t sparse_sample_grad = TA_idx(sparse_sample_grads, thread_num, sparse_array_t);
	sparse_sample_grad.len = sparse_sample_X.len;
	gradient(LOCAL_ITERATE, sparse_sample_X, sample_y, &sparse_sample_grad);

	// Update coordinate individually only over the segmented indices
	int lowest_index = data->num_features / num_threads * thread_num;
	int highest_index = data->num_features / num_threads * (thread_num + 1);
	for (int i = 0; i < sparse_sample_grad.len; i++) {
		int index    = sparse_sample_grad.pts[i].index;
		double value = sparse_sample_grad.pts[i].value;
		if (index >= lowest_index && index < highest_index) {
			TA_idx(LOCAL_ITERATE, index, double) -= get_stepsize()*value;
		}
	}

	// Write local results back to global iterate and get updates
	//    from other indices periodically
	THREAD_COUNTER += 1;
	if (THREAD_COUNTER % GLOBAL_UPDATE_FREQ == 0) {
		for (int i = 0; i < data->num_features; i++) {
			if (i >= lowest_index && i < highest_index) {
				// Write local value to global
				TA_idx(iterate, i, double) = TA_idx(LOCAL_ITERATE, i, double);
			} else {
				// Copy global value locally
				TA_idx(LOCAL_ITERATE, i, double) = TA_idx(iterate, i, double);
			}
		}
	}

	return 0;
}


int segmented_hogwild_initialize(int num_features, int num_thr) {
	num_threads = num_thr;
	srand(time(NULL));
	// Initialize sparse_sample_grads
	malloc_thread_array(&sparse_sample_grads, num_threads);
	for (int n = 0; n < num_threads; n++) {
		TA_idx(sparse_sample_grads, n, sparse_array_t).len = 0;
		TA_idx(sparse_sample_grads, n, sparse_array_t).pts = (sparse_point_t *) malloc(num_features * sizeof(sparse_point_t));
	}
	// Initialize RNG seed states
	malloc_thread_array(&rng_seedp, num_thr);
	for (int n = 0; n < num_thr; n++) {
		TA_idx(rng_seedp, n, unsigned int) = rand() + n;
	}
	// Initialize thread_counters
	malloc_thread_array(&thread_counters, num_thr);
	// Initialize local_iterates
	malloc_thread_array(&local_iterates, num_thr);
	for (int n = 0; n < num_thr; n++) {
		malloc_thread_array(&TA_idx(local_iterates, n, thread_array_t), num_features);
	}
	return 0;
}


int segmented_hogwild_deinitialize(void) {
	// Free sparse_sample_grads
	for (int n = 0; n < num_threads; n++) {
		free(TA_idx(sparse_sample_grads, n, sparse_array_t).pts);
	}
	free_thread_array(&sparse_sample_grads);
	// Free RNG seed states
	free_thread_array(&rng_seedp);
	// Initialize thread_counters
	free_thread_array(&thread_counters);
	// Initialize local_iterates
	free_thread_array(&local_iterates);
	for (int n = 0; n < num_threads; n++) {
		free_thread_array(&TA_idx(local_iterates, n, thread_array_t));
	}
	return 0;
}
