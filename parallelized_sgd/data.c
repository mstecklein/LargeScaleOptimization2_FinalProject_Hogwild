#include <stdlib.h>
#include <stdio.h>
#include "data.h"
#include "psgd_analysis.h"


static int create_sparse_mat(double **M, sparse_array_t **M_sparse, int num_rows, int num_cols) {
	*M_sparse = (sparse_array_t *) malloc(num_rows * sizeof(sparse_array_t));
	for (int r = 0; r < num_rows; r++) {
		// Get num nonzero entries in row
		int num_nonzero = 0;
		for (int c = 0; c < num_cols; c++) {
			if (M[r][c]) {
				num_nonzero++;
			}
		}
		// Fill structure
		(*M_sparse)[r].pts = alloc_sparse_array(num_nonzero);
		(*M_sparse)[r].len = num_nonzero;
		int index = 0;
		for (int c = 0; c < num_cols; c++) {
			if (M[r][c] != 0.0) {
				(*M_sparse)[r].pts[index].index = c;
				(*M_sparse)[r].pts[index].value = M[r][c];
				index++;
			}
		}
	}
	return 0;
}


int read_and_alloc_data(char *filename, data_t *data) {
	// TODO redo this to read from filename


	/*
    double X_data[20] = {0,1,0,0,
                        3,1,0,-2,
                        0,4,0,0,
                        0,0,1,1,
                        -1,0,-3,0};
    double y_data[5] = {-2,-1,0,1,2};
	data->num_samples = 5;
	data->num_features = 4;
	//data->optimal_iterate = ;
	data->X = (double **) malloc(5*sizeof(double **));
	for (int i = 0; i < 5; i++) {
		data->X[i] = (double *) malloc(4*sizeof(double *));
		for (int j = 0; j < 4; j++)
			data->X[i][j] = X_data[4*i + j];
	}
	data->y = (double *) malloc(5*sizeof(double));
	for (int i = 0; i < 5; i++)
		data->y[i] = y_data[i];
	*/


	/*
    double X_data[20] = {0,1,0,0,
                        3,1,0,-2,
                        0,4,0,0,
                        0,0,1,1,
                        -1,0,-3,0};
	double opt_iter[4] = {1, 1, 1, 1};
    double y_data[5] = {1, 2, 4, 2, -4};
	data->num_samples = 5;
	data->num_features = 4;
	data->optimal_iterate = (double *) malloc(data->num_features*sizeof(double));
	for (int i = 0; i < data->num_features; i++)
		data->optimal_iterate[i] = opt_iter[i];
	data->X = (double **) malloc(data->num_samples*sizeof(double **));
	for (int i = 0; i < data->num_samples; i++) {
		data->X[i] = (double *) malloc(data->num_features*sizeof(double *));
		for (int j = 0; j < data->num_features; j++)
			data->X[i][j] = X_data[data->num_features*i + j];
	}
	data->y = (double *) malloc(data->num_samples*sizeof(double));
	for (int i = 0; i < data->num_samples; i++)
		data->y[i] = y_data[i];
	*/


    double X_data[20] ={1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0,0,0,1,
                        1,0,0,1};
	double opt_iter[4] = {1, 1, 1, 1};
    double y_data[5] = {1, 1, 1, 1, 2};
	data->num_samples = 5;
	data->num_features = 4;
	data->optimal_iterate = (double *) malloc(data->num_features*sizeof(double));
	for (int i = 0; i < data->num_features; i++)
		data->optimal_iterate[i] = opt_iter[i];
	data->X = (double **) malloc(data->num_samples*sizeof(double *));
	for (int r = 0; r < data->num_samples; r++) {
		data->X[r] = (double *) malloc(data->num_features*sizeof(double));
		for (int c = 0; c < data->num_features; c++) {
			data->X[r][c] = X_data[r*data->num_features + c];
		}
	}
	data->y = (double *) malloc(data->num_samples*sizeof(double));
	for (int i = 0; i < data->num_samples; i++)
		data->y[i] = y_data[i];


	create_sparse_mat(data->X, &(data->sparse_X), data->num_samples, data->num_features);
	return 0;
}

int dealloc_data(data_t *data) {
	// TODO
	return -1;
}


sparse_point_t * alloc_sparse_array(int len) {
	return (sparse_point_t *) malloc(len*sizeof(sparse_point_t));
}
