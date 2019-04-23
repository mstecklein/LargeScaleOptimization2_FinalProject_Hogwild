#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "data.h"
#include "problem.h"
#include "psgd_analysis.h"
#include "linear_regression.h"
#include "hogwild.h"




int read_data_from_file(char *filename, data_t *data);
int write_results_to_file(int num_threads, log_t *log, timerstats_t *main_thread_stats, timerstats_t **threads_stats);




int main(int argc, char **argv) {
	int rc;
	int max_num_threads;
	char *filename;
	data_t data;
	log_t log;

	// First argument is max number of threads to run,
	//    second argument is max number of threads to run
	if (argc < 3) {
		printf("Usage: ./run <max_num_threads> <data_filename>\n");
		exit(-1);
	}
	max_num_threads = atoi(argv[1]);
	filename = argv[2];

	// Read data file
	rc = read_data_from_file(filename, &data);
	if (rc)
		exit(-1);

	// Run psgd algorithm for various numbers of threads
	log_initialize(&log, data.num_features);
	for (int num_threads = 1; num_threads < max_num_threads; num_threads++) {
		problem_t problem;
		timerstats_t main_thread_stats;
		timerstats_t *threads_stats = (timerstats_t *) malloc(num_threads * sizeof(timerstats_t));

		// Run general analysis for LinearRegression with HOGWILD!
		// TODO
		problem.gradient = linreg_gradient;
		problem.algo_update_func = hogwild;
		problem.algo_init_func = hogwild_initialize;
		problem.algo_deinit_func = hogwild_deinitialize;
		set_current_problem(problem);
		run_psgd_general_analysis(num_threads, &data, &log, &main_thread_stats, &threads_stats);

		// Write results to file
		rc = write_results_to_file(num_threads, &log, &main_thread_stats, &threads_stats);
		if (rc)
			exit(-1);

		free(threads_stats);
	}
	log_free(&log);

}





int read_data_from_file(char *filename, data_t *data) {
	return -1; // TODO
}


int write_results_to_file(int num_threads, log_t *log, timerstats_t *main_thread_stats, timerstats_t **threads_stats) {
	return -1; // TODO
}
