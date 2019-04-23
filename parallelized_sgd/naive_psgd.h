/*
 * Naive algorithm to parallelize SGD using locks.
 */

#ifndef _naive_psgd_h_
#define _naive_psgd_h_

#include "timer.h"


extern timer_t naivepsgd_gradient_timer;
extern timer_t naivepsgd_coordupdate_timer;


int naive_psgd(double *iterate, double **X, double *y);


#endif // _naive_psgd_h_
