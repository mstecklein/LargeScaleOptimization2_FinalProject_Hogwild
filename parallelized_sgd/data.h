#ifndef _data_h_
#define _data_h_


// Sparse matrices can be written in a more compact structure as follows:
//   For each row of the dense matrix, create a list of only the non-zero
//   values of that row. Each entry of the list is a "point" as defined
//   below, which has a column index and value.

typedef struct _sparse_point_t {
	int index;
	double value;
} sparse_point_t;

typedef struct _sparse_array_t {
	int len;
	sparse_point_t *pts;
} sparse_array_t;


typedef struct _data_t {
	double **X; // ptr to data matrix, num_samples x num_features
	sparse_array_t *sparse_X; // sparse representation of X as described above
	double *y; // ptr to label array, num_samples x 1
	double *optimal_iterate; // the true beta for X*beta = y
	int num_samples; // number of data samples
	int num_features; // dimension of feature set
} data_t;


int read_and_alloc_data(char *filename, data_t *data);

int dealloc_data(data_t *data);

sparse_point_t * alloc_sparse_array(int len);

void show_data(data_t *data);


#endif // _data_h_
