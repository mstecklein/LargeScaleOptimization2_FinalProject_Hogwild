#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "problem.h"
#include "psgd_analysis.h"


#define THREADJOB_NONE				0
#define THREADJOB_RECORD_ITERATES	1


/*
 *  Analysis and algorithm wrappers
 */


#define THREADJOB_NONE				0
#define THREADJOB_RECORD_ITERATES	1


pthread_cond_t sync_start_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t sync_start_mutex = PTHREAD_MUTEX_INITIALIZER;


typedef struct _algowrapperargs_t {
	int num_iters;
	int thread_num;
	int threadjob;
	log_t *log;
	data_t *data;
	pthread_cond_t *sync_cond;
	pthread_mutex_t *sync_mutex;
	double *iterate; // array of length data->num_features
} algowrapperargs_t;


static void* algo_wrapper(void *wrapperargs) {
	int rc;
	int log_step; // # iters between logging
	timer_t timer;
	algowrapperargs_t *args = (algowrapperargs_t *) wrapperargs;
	log_step = args->num_iters / NUM_LOG_POINTS;
	// timer_initialize(&timer, TIMER_SCOPE_THREAD);
	timer_initialize(&timer, TIMER_SCOPE_PROCESS); // TODO change this to thread

	// Wait at starting line for release by condition
	pthread_mutex_lock(&sync_start_mutex);
    pthread_cond_wait(&sync_start_cond, &sync_start_mutex);
    pthread_mutex_unlock(&sync_start_mutex); // unlocking for other threads
	timer_start(&timer);

	// Run algo for num_iters iterations
	for (int i = 1; i <= args->num_iters; i++) {
		rc = current_problem.algo_update_func(args->iterate, args->data, args->thread_num);
		if (rc)
			pthread_exit(NULL);
		// Log iterate and timestamp values, if required
		if (args->threadjob == THREADJOB_RECORD_ITERATES) {
			if (i % log_step == 0) {
				int sz = args->log->size;
				if (sz+1 <= args->log->capacity) {
					// copy iterate value
					for (int j = 0; j < args->data->num_features; j++) {
						args->log->iterates[sz][j] = args->iterate[j];
					}
					// copy timer
					args->log->timestamps[sz] = timer;
					// inc log size
					args->log->size++;
				}
			}
		}
	}
	
	timer_deinitialize(&timer);
	pthread_exit(NULL);
}


int run_psgd_general_analysis(int num_threads, data_t *data, log_t *log, timerstats_t *main_thread_stats, timerstats_t **threads_stats) {
	int rc;
	algowrapperargs_t args;
	pthread_t *threads;
	pthread_attr_t attr;
	void *status;
	timer_t main_thread_timer;
	pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t sync_cond = PTHREAD_COND_INITIALIZER;

	// Alloc, initialize, and set thread joinable
	threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	timer_initialize(&main_thread_timer, TIMER_SCOPE_PROCESS);
	rc = current_problem.algo_init_func(data->num_features, num_threads);
	if (rc)
		return rc;

	// Start wrapper threads
	args.num_iters = NUM_TOTAL_ITER / num_threads;
	args.data = data;
	args.sync_cond = &sync_cond;
	args.sync_mutex = &sync_mutex;
	args.iterate = (double *) malloc(data->num_features*sizeof(double));
	memset(args.iterate, 0, data->num_features*sizeof(double));
	for (int thread_num = 0; thread_num < num_threads; thread_num++) {
		args.thread_num = thread_num;
		if (!thread_num) {
			args.threadjob = THREADJOB_RECORD_ITERATES;
			args.log = log;
		} else {
			args.threadjob = THREADJOB_NONE;
			args.log = NULL;
		}
		rc = pthread_create(&threads[thread_num], &attr, algo_wrapper, (void *)&args);
		if (rc)
			return rc;
	}

	// Start main thread's timer and release threads
	sleep(1); // wait for threads to hit condition
	timer_start(&main_thread_timer);
	rc = pthread_cond_broadcast(&sync_start_cond);
	if (rc)
		return rc;

	// Wait for threads to finish
	for (int thread_num = 0; thread_num < num_threads; thread_num++) {
		rc = pthread_join(threads[thread_num], &status);
		if (rc)
			return rc;
	}

	// Stop main thread timer
	timer_pause(&main_thread_timer);
	timer_get_stats(&main_thread_timer, main_thread_stats);

	// Clean up
	free(threads);
	timer_deinitialize(&main_thread_timer);
	pthread_attr_destroy(&attr);
	current_problem.algo_deinit_func();
	return 0;
}




/*
 *  Logging
 */


int log_initialize(log_t *log, int num_data_features) {
	// Alloc iterates array
	log->iterates = (double **) malloc(NUM_LOG_POINTS*sizeof(double *));
	// Alloc each iterate
	for (int i = 0; i < NUM_LOG_POINTS; i++) {
		int iterate_size = num_data_features*sizeof(double);
		double *iterate = (double *) malloc(iterate_size);
		memset(iterate, 0, iterate_size);
		log->iterates[i] = iterate;
	}
	// Alloc timestamp array
	log->timestamps = (timer_t *) malloc(NUM_LOG_POINTS*sizeof(timer_t));
	log->size = 0;
	log->capacity = NUM_LOG_POINTS;
	log->num_data_features = num_data_features;
	return 0;
}


int log_free(log_t *log) {
	// Free each iterate
	for (int i = 0; i < NUM_LOG_POINTS; i++) {
		free(log->iterates[i]);
	}
	// Free iterates array
	free(log->iterates);
	// Free timestamp array
	free(log->timestamps);
	return 0;
}




/*
 *  Debug printouts
 */


void print_dense_array(double *arr, int len) {
	printf("[ ");
	for (int i = 0; i < len; i++) {
		printf("%f ", arr[i]);
	}
	printf("]\n");
}


void print_sparse_array(sparse_array_t *arr) {
	printf("[ ");
	for (int i = 0; i < arr->len; i++) {
		printf("%d:%f ", arr->pts[i].index, arr->pts[i].value);
	}
	printf("]\n");
}
