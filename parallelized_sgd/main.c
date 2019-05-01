#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "data.h"
#include "problem.h"
#include "psgd_analysis.h"
#include "linear_regression.h"
#include "hogwild.h"






int main(int argc, char **argv) {
	int rc;
	int num_threads;
	char *filename;
	data_t data;
	log_t log;

	// First argument is number of threads to run,
	//    second argument is the data filename,
	//    third argument is whether to track gradient
	//    and coordinate update stats (Any argument here
	//    sets it to true, defaults to false. Tracking
	//    these stats is very intrusive).
	if (argc < 3) {
		printf("Usage: ./run <num_threads> <data_filename> [record gradient/update stats]\n");
		exit(-1);
	}
	num_threads = atoi(argv[1]);
	filename = argv[2];
	if (argc >= 4) {
		set_track_gradient_coordupdate(1);
	} else {
		set_track_gradient_coordupdate(0);
	}

	// Read data file
	rc = read_and_alloc_data(filename, &data);
	if (rc)
		exit(-1);

	// Run psgd algorithm for various problems
	problem_t problem;

	// Initialize
	log_initialize(&log, data.num_features);
	timerstats_t main_thread_stats;
	timerstats_t *threads_stats = (timerstats_t *) malloc(num_threads * sizeof(timerstats_t));
	timerstats_t *gradient_stats = (timerstats_t *) malloc(num_threads * sizeof(timerstats_t));
	timerstats_t *coord_update_stats = (timerstats_t *) malloc(num_threads * sizeof(timerstats_t));

	// Run general analysis for LinearRegression with HOGWILD!
	problem.gradient = linreg_gradient;
	problem.algo_update_func = hogwild;
	problem.algo_init_func = hogwild_initialize;
	problem.algo_deinit_func = hogwild_deinitialize;
	problem.stepsize = 0.000001;
	set_current_problem(problem);
	rc = run_psgd_general_analysis(num_threads, &data, &log, &main_thread_stats, threads_stats, gradient_stats, coord_update_stats);
	if (rc) {
		printf("Error running general analysis for LinearRegression with HOGWILD!\n");
		exit(rc);
	}

	// Write results to file
	rc = write_results_to_file(num_threads, &log, main_thread_stats, threads_stats, gradient_stats, coord_update_stats);
	if (rc)
		exit(-1);

	free(threads_stats);
	free(gradient_stats);
	free(coord_update_stats);
	log_free(&log);
	dealloc_data(&data);
}
