#include<pthread.h>
#include "data.h"
#include "problem.h"
#include "naive_psgd.h"


sparse_array_t sparse_sample_grad;
pthread_mutex_t lock;


int naive_psgd(thread_array_t iterate, data_t *data, int thread_num) {
	// Get random sample
	int rand_index = rand() % data->num_samples;
	sparse_array_t sparse_sample_X = data->sparse_X[rand_index];
	double sample_y = data->y[rand_index];

	// Lock
	pthread_mutex_lock(&lock);

	// Evaluate gradient
	gradient(iterate, sparse_sample_X, sample_y, &sparse_sample_grad, NULL);
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
	int rc;
	sparse_sample_grad.len = 0;
	sparse_sample_grad.pts = (sparse_point_t *) malloc(num_features * sizeof(sparse_point_t));
	return pthread_mutex_init(&lock, NULL);
}


int naive_psgd_deinitialize(void) {
	free(sparse_sample_grad.pts);
	pthread_mutex_destroy(&lock);
	return 0;
}
