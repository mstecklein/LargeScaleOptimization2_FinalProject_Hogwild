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


// Condition vars to start all threads at the same time
pthread_cond_t sync_start_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t sync_start_mutex = PTHREAD_MUTEX_INITIALIZER;


typedef struct _algowrapperargs_t {
	int num_iters;
	int thread_num;
	int threadjob;
	log_t *log;
	timerstats_t *threadstats;
	data_t *data;
	pthread_cond_t *sync_cond;
	pthread_mutex_t *sync_mutex;
	double *iterate; // array of length data->num_features
} algowrapperargs_t;


static void* algo_wrapper(void *wrapperargs) {
	int rc;
	int log_step; // # iters between logging
	ttimer_t timer;
	algowrapperargs_t *args = (algowrapperargs_t *) wrapperargs;
	log_step = args->num_iters / NUM_LOG_POINTS;
#ifdef __linux__
	timer_initialize(&timer, TIMER_SCOPE_THREAD);
#endif // __linux__
#ifdef __APPLE__
	timer_initialize(&timer, TIMER_SCOPE_PROCESS);
#endif // __APPLE__

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
					timer_pause(&timer);
					args->log->timestamps[sz] = timer;
					timer_start(&timer);
					// inc log size
					args->log->size++;
				}
			}
		}
	}
	
	// Handle this thread's timer and stats
	timer_pause(&timer);
	timer_get_stats(&timer, args->threadstats);
	timer_deinitialize(&timer);

	pthread_exit(NULL);
}


int run_psgd_general_analysis(int num_threads, data_t *data, log_t *log, timerstats_t *main_thread_stats, timerstats_t *threads_stats_array, timerstats_t *gradient_stats_array, timerstats_t *coord_update_stats_array) {
	int rc;
	algowrapperargs_t *args;
	pthread_t *threads;
	pthread_attr_t attr;
	void *status;
	ttimer_t main_thread_timer;
	pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t sync_cond = PTHREAD_COND_INITIALIZER;

	// Initialize, alloc, and set thread joinable
	args = (algowrapperargs_t *) malloc(num_threads * sizeof(algowrapperargs_t));
	threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));
	pthread_attr_init(&attr);	
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	timer_initialize(&main_thread_timer, TIMER_SCOPE_PROCESS);
	rc = current_problem.algo_init_func(data->num_features, num_threads);
	if (rc)
		return rc;

	// Start wrapper threads
	for (int thread_num = 0; thread_num < num_threads; thread_num++) {
		// Fill args for this thread
		args[thread_num].num_iters = NUM_TOTAL_ITER / num_threads;
		args[thread_num].thread_num = thread_num;
		if (!thread_num) {
			args[thread_num].threadjob = THREADJOB_RECORD_ITERATES;
			args[thread_num].log = log;
		} else {
			args[thread_num].threadjob = THREADJOB_NONE;
			args[thread_num].log = NULL;
		}
		args[thread_num].threadstats = &threads_stats_array[thread_num];
		args[thread_num].data = data;
		args[thread_num].sync_cond = &sync_cond;
		args[thread_num].sync_mutex = &sync_mutex;
		args[thread_num].iterate = (double *) malloc(data->num_features*sizeof(double));
		memset(args[thread_num].iterate, 0, data->num_features*sizeof(double));
		// Create thread with args
		rc = pthread_create(&threads[thread_num], &attr, algo_wrapper, (void *)&args[thread_num]);
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

	// Get gradient / coord updt stats
	timer_get_internal_timer_stats(gradient_stats_array, coord_update_stats_array);

	// Clean up
	free(threads);
	timer_deinitialize(&main_thread_timer);
	pthread_attr_destroy(&attr);
	current_problem.algo_deinit_func();
	return 0;
}


static int track_gradientupdate;

void set_track_gradient_coordupdate(int true_false) {
	track_gradientupdate = true_false;
}

int track_gradient_coordupdate(void) {
	return track_gradientupdate;
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
	log->timestamps = (ttimer_t *) malloc(NUM_LOG_POINTS*sizeof(ttimer_t));
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





//
// Writing results to files:::
//


static int write_results_log(int num_threads, log_t *log) {
	FILE *fp;
	char filename[30];
	sprintf(filename, "log_%dthreads.csv", num_threads);
	fp = fopen(filename,"w");
	if (!fp)
		return -1;
	// Write header
	fprintf(fp, "Time, Iterate\n");
	// Write each log
	double start_cumulative = log->timestamps[0].real_cumulative;
	for (int i = 0; i < NUM_LOG_POINTS; i++) {
		// Write real time elapsed
		double time_elapsed = timer_get_elapsed(start_cumulative, log->timestamps[i].real_cumulative);
		fprintf(fp, "%f, ", time_elapsed);
		// Write iterate
		fprintf(fp, "[");
		for (int j = 0; j < log->num_data_features; j++) {
			fprintf(fp, "%f", log->iterates[i][j]);
			if (j != log->num_data_features-1) {
				fprintf(fp, ", ");
			}
		}
		fprintf(fp, "]");
		fprintf(fp, "\n");
	}
	fclose(fp);
	return 0;
}


static int write_results_threads_stats(int num_threads, timerstats_t main_thread_stats, timerstats_t *threads_stats_arr) {
	FILE *fp;
	char filename[30];
	sprintf(filename, "threadstats_%dthread.csv", num_threads);
	fp = fopen(filename,"w");
	if (!fp)
		return -1;
	// Write header
	fprintf(fp, "Threadname, Real, User, Sys\n");
	// Write main thread stats
	fprintf(fp, "Main, %f, %f, %f\n", main_thread_stats.real, main_thread_stats.user, main_thread_stats.sys);
	// Write other threads stats
	for (int i = 0; i < num_threads; i++) {
		fprintf(fp, "Thread%d, %f, %f, %f\n", i, threads_stats_arr[i].real, threads_stats_arr[i].user, threads_stats_arr[i].sys);
	}
	fclose(fp);
	return 0;
}


static int write_results_grad_coord(int num_threads, timerstats_t *gradient_stats_array, timerstats_t *coord_update_stats_array) {
	FILE *fp;
	char filename[30];
	sprintf(filename, "gradcoord_%dthread.csv", num_threads);
	fp = fopen(filename,"w");
	if (!fp)
		return -1;
	if (track_gradient_coordupdate()) {
		// Write header
		fprintf(fp, "Threadname, Grad_Real, Grad_User, Grad_Sys, Coord_Real, Coord_User, Coord_Sys\n");
		// Write threads stats
		for (int i = 0; i < num_threads; i++) {
			fprintf(fp, "Thread%d, %f, %f, %f, %f, %f, %f\n", i,
					gradient_stats_array[i].real,
					gradient_stats_array[i].user,
					gradient_stats_array[i].sys,
					coord_update_stats_array[i].real,
					coord_update_stats_array[i].user,
					coord_update_stats_array[i].sys);
		}
	} else {
		// Write note that we didn't track this info
		fprintf(fp, "No stats available. Did not track them.");
	}
	fclose(fp);
	return 0;
}


int write_results_to_file(int num_threads, log_t *log, timerstats_t main_thread_stats, timerstats_t *threads_stats_array, timerstats_t *gradient_stats_array, timerstats_t *coord_update_stats_array) {
	int rc;
	rc = write_results_log(num_threads, log);
	if (rc)
		return rc;
	rc = write_results_threads_stats(num_threads, main_thread_stats, threads_stats_array);
	if (rc)
		return rc;
	rc = write_results_grad_coord(num_threads, gradient_stats_array, coord_update_stats_array);
	if (rc)
		return rc;
	return 0;
}
