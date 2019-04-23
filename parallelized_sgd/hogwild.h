/*
 * HOGWILD! algorithm from TODO
 */

#ifndef _hogwild_h_
#define _hogwild_h_

#include "timer.h"


extern timer_t hogwild_gradient_timer;
extern timer_t hogwild_coordupdate_timer;
extern timer_t hogwild_coordupdate_repeat_timer;


int hogwild(double *iterate, data_t *data);


int hogwild_initialize(int num_features);
int hogwild_deinitialize(void);


#endif // _hogwild_h_
