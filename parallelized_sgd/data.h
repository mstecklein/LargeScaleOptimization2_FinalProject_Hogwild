/*
 * TODO
 */

#ifndef _data_h_
#define _data_h_


typedef struct _data_t {
	double **X; // ptr to data matrix, num_samples x num_features
	double *y; // ptr to label array, num_samples x 1
	double *optimal_iterate; // the true beta for X*beta = y
	int num_samples; // number of data samples
	int num_features; // dimension of feature set
} data_t;


int read_and_alloc_data(data_t *data);


int dealloc_data(data_t *data);


#endif // _data_h_
