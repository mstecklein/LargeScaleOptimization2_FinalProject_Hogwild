/*
 * Example algorithm to show the effects of shared and independent
 * variables on the parallelization of an algorithm, even when locks
 * are not used.
 *
 * This algorithm either explicitly partitions variables across
 * threads (update independent) or allows different threads to
 * randomly update common variables (update shared).
 */

#ifndef _example_algo_h_
#define _example_algo_h_

#include "data.h"
#include "thread_array.h"


int example_algo_update_independent(thread_array_t iterate, data_t *data, int thread_num);
int example_algo_update_shared(thread_array_t iterate, data_t *data, int thread_num);

int example_algo_initialize(int num_features, int num_threads);
int example_algo_deinitialize(void);


#endif // _example_algo_h_
