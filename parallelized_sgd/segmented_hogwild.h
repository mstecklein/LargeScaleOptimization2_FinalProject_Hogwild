/*
 * Vartion on HOGWILD! algorithm from "HOGWILD!: A Lock-Free Approach to Parallelizing Stochastic Gradient Descent" by Niu et. al
 *
 *    This variation overcomes the problem described in example_algo for the HOGWILD! algorithm by segmenting the iterate
 *    into independent partions, where each thread only updates its assigned partition. Each thread keeps a local copy of the iterate
 *    that it works on, and periodically updates the global iterate over the partition it owns and updates its local iterate for all
 *    other partitions. This algorithm empirically seems to converage at a similar rate to HOGWILD! with much better speedups.
 */

#ifndef _segmented_hogwild_h_
#define _segmented_hogwild_h_

#include "timer.h"
#include "thread_array.h"


// Frequency at which the algorithm syncs its local iterate with the global iterate
#define GLOBAL_UPDATE_FREQ	1000


int segmented_hogwild(thread_array_t iterate, data_t *data, int thread_num);


int segmented_hogwild_initialize(int num_features, int num_threads);
int segmented_hogwild_deinitialize(void);


#endif // _segmented_hogwild_h_
