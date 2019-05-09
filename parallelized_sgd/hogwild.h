/*
 * HOGWILD! algorithm from "HOGWILD!: A Lock-Free Approach to Parallelizing Stochastic Gradient Descent" by Niu et. al
 */

#ifndef _hogwild_h_
#define _hogwild_h_

#include "timer.h"
#include "thread_array.h"


int hogwild(thread_array_t iterate, data_t *data, int thread_num);


int hogwild_initialize(int num_features, int num_threads);
int hogwild_deinitialize(void);


// This will be a lower bound on the number of collisions in
// atomic increment. Since it is not made thread-safe, threads
// may overwrite each others' increments.
extern unsigned int hogwild_num_atomic_dec_collisions;


#endif // _hogwild_h_
