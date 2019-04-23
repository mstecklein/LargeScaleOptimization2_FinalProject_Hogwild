#include <time.h>
#include <stdlib.h>
#include "data.h"
#include "problem.h"
#include "hogwild.h"


timer_t hogwild_gradient_timer;
timer_t hogwild_coordupdate_timer;
timer_t hogwild_coordupdate_repeat_timer;
static double *sample_grad;


static void atomic_decrement(double *dest, double dec_amt) {
	double ret_val = 0.0;
	double orig_val = 1.0;
	double dec_val;
	// Perform a compare-and-swap. If the update was of dest was
	//   atomic, the returned value should match the orig_val's
	//   read of the destination.
	while (ret_val != orig_val) {
		orig_val = *dest;
		dec_val = orig_val - dec_amt;
		__atomic_exchange(dest, &dec_val, &ret_val, __ATOMIC_RELAXED);
	}
}


int hogwild(double *iterate, data_t *data) {
	// Get random sample
	int rand_index = rand() % data->num_samples;
	double *sample_x = data->X[rand_index];
	double sample_y = data->y[rand_index];
	// TODO get sample support too

	// Read iterate
	// TODO is it necessary to copy the gradient value here?

	// Evaluate gradient
	timer_start(&hogwild_gradient_timer);
	gradient(iterate, sample_x, sample_y, sample_grad);
	timer_pause(&hogwild_gradient_timer);

	// Update coordinate individually and atomically
	timer_initialize(&hogwild_coordupdate_timer, TIMER_SCOPE_THREAD);
	for (int i = 0; i < data->num_features; i++) { // TODO modify this to only update on the support of the sample
		atomic_decrement(&iterate[i], get_stepsize()*sample_grad[i]);
	}
	timer_pause(&hogwild_coordupdate_timer);

	return 0;
}


int hogwild_initialize(int num_features) {
	timer_initialize(&hogwild_gradient_timer, TIMER_SCOPE_THREAD);
	timer_initialize(&hogwild_coordupdate_timer, TIMER_SCOPE_THREAD);
	timer_initialize(&hogwild_coordupdate_repeat_timer, TIMER_SCOPE_THREAD);
	srand(time(NULL));
	sample_grad = (double *) malloc(num_features * sizeof(double));
	return 0;
}


int hogwild_deinitialize(void) {
	free(sample_grad);
	return 0;
}
