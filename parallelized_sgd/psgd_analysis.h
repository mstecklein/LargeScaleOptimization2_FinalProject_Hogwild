/*
 * Parallel Stochastic Gradient Descent implementation and analysis.
 */

#ifndef _psgd_h_
#define _psgd_h_


#include "data.h"
#include "timer.h"


// Logging structure
typedef struct _log_t {
	double **iterates; // array of len get_current_problem().num_log_points, where each element is an array of doubles of length num_data_features
	int num_data_features; // len of iterate
	ttimer_t *timestamps; // array of len get_current_problem().num_log_points
	int size; // number of elements in the log
	int capacity;
} log_t;

int log_initialize(log_t *log, int num_data_features);
int log_free(log_t *log);


// Analysis
int run_psgd_general_analysis(int num_threads, data_t *data, log_t *log, timerstats_t *main_thread_stats, timerstats_t *threads_stats_array, timerstats_t *gradient_stats_array, timerstats_t *coord_update_stats_array);

extern int track_gradientupdate;

static inline void set_track_gradient_coordupdate(int true_false) {
	track_gradientupdate = true_false;
}

static inline int track_gradient_coordupdate(void) {
	return track_gradientupdate;
}

int create_results_dir(char *input_filename, char *ret_results_dir);

int write_results_to_file(int num_threads, char *results_dir, log_t *log, timerstats_t main_thread_stats, timerstats_t *threads_stats_array, timerstats_t *gradient_stats_array, timerstats_t *coord_update_stats_array);


// Debug printouts
void print_dense_array(double *arr, int len);
void print_sparse_array(sparse_array_t *arr);


#endif // _psgd_h_
