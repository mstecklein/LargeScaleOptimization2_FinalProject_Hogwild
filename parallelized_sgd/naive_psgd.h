/*
 * Naive algorithm to parallelize SGD using locks.
 */

#ifndef _naive_psgd_h_
#define _naive_psgd_h_

#include "thread_array.h"


int naive_psgd(thread_array_t iterate, data_t *data, int thread_num);

int naive_psgd_initialize(int num_features, int num_threads);
int naive_psgd_deinitialize(void);


#endif // _naive_psgd_h_
