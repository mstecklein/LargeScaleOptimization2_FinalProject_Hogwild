/*
 * A structure to use when multiple threads need to read/write an
 * array. This structure spaces the data so prevent thread inter-
 * dependencies during writes.
 * 
 * We observed that writes to memory locations that are within the
 * same contiguous 32 byte chunk were causing thread inter-dependencies.
 * To solve this, this module functions as a wrapper around any given type
 * array (<type> *) to force a buffer of space between its entries.
 */

#ifndef _thread_array_
#define _thread_array_


#include <stdlib.h>
#include <string.h>


// Use to index into a thread array (TA):
#define TA_idx(thr_arr, i, type)	(*((type *) &(thr_arr[i].data)))


// The number of bytes to offset data by
#define DATA_SPACING	32


// Use to make sure the type is large enough
#define TA_valid_type(type)		((sizeof(type) <= DATA_SPACING) ? 1 : 0)


typedef struct data_chunk {
	char data[DATA_SPACING];
} data_chunk_t;


typedef data_chunk_t*	thread_array_t;


static inline void malloc_thread_array(thread_array_t *arr, int len) {
	int size = len * sizeof(data_chunk_t);
	*arr = (data_chunk_t *) malloc(size);
	memset(*arr, 0, size);
}

static inline void free_thread_array(thread_array_t *arr) {
	free(*arr);
}


#endif // _thread_array_
