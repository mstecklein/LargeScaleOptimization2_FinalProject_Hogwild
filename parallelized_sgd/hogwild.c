#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "data.h"
#include "problem.h"
#include "psgd_analysis.h"
#include "hogwild.h"


static sparse_array_t *sparse_sample_grads;
static double **scratchpad;
static int num_threads;


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
		if (ret_val != orig_val) // TODO remove this
			printf("A collision occurred in atomic_decrement\n"); 
	} while (ret_val != orig_val);
}


int hogwild(double *iterate, data_t *data, int thread_num) {
	// Get random sample
	int rand_index = rand() % data->num_samples;
	sparse_array_t sparse_sample_X = data->sparse_X[rand_index];
	double sample_y = data->y[rand_index];

	// Evaluate gradient
	sparse_sample_grads[thread_num].len = sparse_sample_X.len;
	gradient(iterate, sparse_sample_X, sample_y, &sparse_sample_grads[thread_num], scratchpad[thread_num]);

	if (rand_index == 20) {
		printf("-----------------------------------\n");
		printf("sparse_X:::\n");
		for (int r = 0; r < 5; r++) {
			printf("\t%d",r);print_sparse_array(&(data->sparse_X[r]));
		}
		printf("Iterate:");print_dense_array(iterate, 4);
		printf("Sparse X:");print_sparse_array(&sparse_sample_X);
		printf("y: %f\n", sample_y);
		printf("Grad:");print_sparse_array(&sparse_sample_grads[thread_num]);
	}

	// Update coordinate individually and atomically
	for (int i = 0; i < sparse_sample_grads[thread_num].len; i++) {
		int index = sparse_sample_grads[thread_num].pts[i].index;
		double value = sparse_sample_grads[thread_num].pts[i].value;
		atomic_decrement(&iterate[index], get_stepsize()*value);
	}
	
	return 0;
}


int hogwild_initialize(int num_features, int num_thr) {
	num_threads = num_thr;
	srand(time(NULL));
	// Initialize sparse_sample_grads
	sparse_sample_grads = (sparse_array_t *) malloc(num_threads*sizeof(sparse_array_t));
	for (int n = 0; n < num_threads; n++) {
		sparse_sample_grads[n].len = 0;
		sparse_sample_grads[n].pts = (sparse_point_t *) malloc(num_features * sizeof(sparse_point_t));
	}
	// Initialize scratchpad
	scratchpad = (double **) malloc(num_threads*sizeof(double *));
	for (int n = 0; n < num_threads; n++) {
		scratchpad[n] = (double *) malloc(num_features*sizeof(double));
	}
	return 0;
}


int hogwild_deinitialize(void) {
	// Free sparse_sample_grads
	for (int n = 0; n < num_threads; n++) {
		free(sparse_sample_grads[n].pts);
	}
	free(sparse_sample_grads);
	// Free scratchpad
	for (int n = 0; n < num_threads; n++) {
		free(scratchpad[n]);
	}
	free(scratchpad);
	return 0;
}
