/*
 * HOGWILD! algorithm from TODO
 */

#ifndef _hogwild_h_
#define _hogwild_h_

#include "timer.h"


int hogwild(double *iterate, data_t *data, int thread_num);


int hogwild_initialize(int num_features, int num_threads);
int hogwild_deinitialize(void);


#endif // _hogwild_h_
