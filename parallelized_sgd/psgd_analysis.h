/*
 * Parallel Stochastic Gradient Descent implementation and analysis.
 */

#ifndef _psgd_h_
#define _psgd_h_


#define NUM_TOTAL_ITER		1000
#define NUM_LOG_POINTS		1 // 20

#include "timer.h"
#include "data.h"


// Logging structure
typedef struct _log_t {
	double **iterates; // array of len NUM_LOG_POINTS, where each element is an array of doubles of length num_features
	timer_t *timestamps; // array of len NUM_LOG_POINTS
	int size; // number of elements in the log
	int capacity;
} log_t;

int log_initialize(log_t *log, int num_data_features);
int log_free(log_t *log);


// Analysis
int run_psgd_general_analysis(int num_threads, data_t *data, log_t *log, timerstats_t *main_thread_stats, timerstats_t **threads_stats);


#endif // _psgd_h_
