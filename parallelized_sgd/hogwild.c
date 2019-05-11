#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "data.h"
#include "problem.h"
#include "thread_array.h"
#include "psgd_analysis.h"
#include "hogwild.h"


// This will be a lower bound on the number of collisions. Since
// it is not made thread-safe, threads may overwrite each others'
// increments.
unsigned int hogwild_num_atomic_dec_collisions = 0;


static thread_array_t /*sparse_array_t*/ sparse_sample_grads;
static int num_threads;
static thread_array_t /*unsigned int*/ rng_seedp; // seed state for random number generator, one for each thread

static ttimer_t *gradient_timers;
static ttimer_t *coord_update_timers;


static void atomic_decrement(double *dest, double dec_amt) {
	double ret_val;
	double orig_val;
	double dec_val;
	// Perform a compare-and-swap. If the update was of dest was
	//   atomic, the returned value should match the orig_val's
	//   read of the destination.
	do {
		orig_val = *dest;
		dec_val = orig_val - dec_amt;
		__atomic_exchange(dest, &dec_val, &ret_val, __ATOMIC_RELAXED);
		if (ret_val != orig_val)
			hogwild_num_atomic_dec_collisions++;
	} while (ret_val != orig_val);
}


int hogwild(thread_array_t iterate, data_t *data, int thread_num) {
	// Get random sample
	//    rand_r is reentrant (thread safe)
	int rand_index = rand_r(&TA_idx(rng_seedp, thread_num, unsigned int)) % data->num_samples;
	sparse_array_t sparse_sample_X = data->sparse_X[rand_index];
	double sample_y = data->y[rand_index];

	// Evaluate gradient
	sparse_array_t sparse_sample_grad = TA_idx(sparse_sample_grads, thread_num, sparse_array_t);
	sparse_sample_grad.len = sparse_sample_X.len;
#ifdef TRACK_GRADIENT_COORDUPDATE
	timer_start(&gradient_timers[thread_num]);
#endif
	gradient(iterate, sparse_sample_X, sample_y, &sparse_sample_grad);
#ifdef TRACK_GRADIENT_COORDUPDATE
	timer_pause(&gradient_timers[thread_num]);
#endif

	// Update coordinate individually and atomically
#ifdef TRACK_GRADIENT_COORDUPDATE
	timer_start(&coord_update_timers[thread_num]);
#endif
	for (int i = 0; i < sparse_sample_grad.len; i++) {
		int index    = sparse_sample_grad.pts[i].index;
		double value = sparse_sample_grad.pts[i].value;
		atomic_decrement(&TA_idx(iterate, index, double), get_stepsize()*value);
	}
#ifdef TRACK_GRADIENT_COORDUPDATE
	timer_pause(&coord_update_timers[thread_num]);
#endif
	
	return 0;
}


int hogwild_initialize(int num_features, int num_thr) {
	num_threads = num_thr;
	srand(time(NULL));
	// Initialize sparse_sample_grads
	malloc_thread_array(&sparse_sample_grads, num_threads);
	for (int n = 0; n < num_threads; n++) {
		TA_idx(sparse_sample_grads, n, sparse_array_t).len = 0;
		TA_idx(sparse_sample_grads, n, sparse_array_t).pts = (sparse_point_t *) malloc(num_features * sizeof(sparse_point_t));
	}
	// Initialize timers
	gradient_timers = (ttimer_t *) malloc(num_thr * sizeof(ttimer_t));
	coord_update_timers = (ttimer_t *) malloc(num_thr * sizeof(ttimer_t));
	for (int n = 0; n < num_threads; n++) {
#ifdef __linux__
		timer_initialize(&gradient_timers[n], TIMER_SCOPE_THREAD);
		timer_initialize(&coord_update_timers[n], TIMER_SCOPE_THREAD);
#endif // __linux__
#ifdef __APPLE__
		timer_initialize(&gradient_timers[n], TIMER_SCOPE_PROCESS);
		timer_initialize(&coord_update_timers[n], TIMER_SCOPE_PROCESS);
#endif // __APPLE__
	}
	// Initialize RNG seed states
	malloc_thread_array(&rng_seedp, num_thr);
	for (int n = 0; n < num_thr; n++) {
		TA_idx(rng_seedp, n, unsigned int) = rand() + n;
	}
	return 0;
}


int hogwild_deinitialize(void) {
	// Free sparse_sample_grads
	for (int n = 0; n < num_threads; n++) {
		free(TA_idx(sparse_sample_grads, n, sparse_array_t).pts);
	}
	free_thread_array(&sparse_sample_grads);
	// Free timers
	for (int n = 0; n < num_threads; n++) {
		timer_deinitialize(&gradient_timers[n]);
		timer_deinitialize(&coord_update_timers[n]);
	}
	free(gradient_timers);
	free(coord_update_timers);
	// Free RNG seed states
	free_thread_array(&rng_seedp);
	// Print lower bound on # of atomic_decrement collisions
	printf("There were at least %d collisions in atomic_decrement\n", hogwild_num_atomic_dec_collisions);
	return 0;
}


int timer_get_internal_timer_stats(timerstats_t *gradient_stats, timerstats_t *coord_update_stats) {
	int rc;
	for (int n = 0; n < num_threads; n++) {
		rc = timer_get_stats(&gradient_timers[n], &gradient_stats[n]);
		if (rc)
			return rc;
		rc = timer_get_stats(&coord_update_timers[n], &coord_update_stats[n]);
		if (rc)
			return rc;
	}
	return 0;
}
