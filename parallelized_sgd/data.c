#include <stdlib.h>
#include <stdio.h>
#include "data.h"
//#include "problem.h"
//#include "psgd_analysis.h"


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

    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open %s", filename);
        return -1;
    }

    //Read in the number of samples and features, respectively
    int n;
    int d;

    if (fscanf(fp, "%d", &n) == EOF){
        if (feof(fp)){
            printf("Problem while parsing %s - EOF reached while reading n", filename);
            return -1;
        }
        else{
            printf("Problem while parsing %s - error occurred while reading n", filename);
            return -1;
        }
    }

    if (fscanf(fp, "%d", &d) == EOF){
        if (feof(fp)){
            printf("Problem while parsing %s - EOF reached while reading d", filename);
            return -1;
        }
        else{
            printf("Problem while parsing %s - error occurred while reading d", filename);
            return -1;
        }
    }

    if(n <= 0 || d <= 0){
        printf("Problem while parsing %s - n or d has illegal value", filename);
        return -1;
    }

    //allocate space for the arrays
    data->optimal_iterate = (double *) malloc(d * sizeof(double));
    data->y = (double *) malloc(n * sizeof(double));
    data->X = (double **) malloc(n * sizeof(double *));
    for(int r = 0; r < n; r++){
        data->X[r] = malloc(d * sizeof(double));
    }

    data->num_samples = n;
	data->num_features = d;

    //populate the optimal iterate
    int c = 0;

    while(c < d && fscanf(fp, "%lf", &(data->optimal_iterate[c])) != EOF){
        c++;
    }
    if(feof(fp)){
        printf("Problem while parsing %s - EOF reached while reading optimal iterate", filename);
        return -1;
    }
    else if(ferror(fp)){
        printf("Problem while parsing %s - error occurred while reading optimal iterate", filename);
        return -1;
    }

    //read and throw away last digit - should be a 0
    double trash;

    if(fscanf(fp, "%lf", &trash) == EOF || trash != 0){
        printf("Problem while parsing %s - error or illegal value while reading trash digit", filename);
        return -1;
    }

    //populate the data and labels
    c=0;
    int r = 0;

    while(r < n){
        while(c < d && fscanf(fp, "%lf", &(data->X[r][c]))!=EOF){
            c++;
        }
        if(ferror(fp) || feof(fp)){
            printf("Problem while parsing %s - error or EOF occurred while reading X", filename);
            return -1;
        }

        if(fscanf(fp, "%lf", &(data->y[r]))==EOF){
            printf("Problem while parsing %s - error or EOF occurred while reading y", filename);
            return -1;
        }

        c = 0;
        r++;
    }

    create_sparse_mat(data->X, &(data->sparse_X), data->num_samples, data->num_features);

    fclose(fp);

    return 0;
}

int dealloc_data(data_t *data) {

	//free the rows of X and sparse X first
	for (int r = 0; r < data->num_samples; r++){
        free(data->X[r]);
        free(data->sparse_X[r].pts);
	}

	//free all the references to arrays
	free(data->X);
	free(data->sparse_X);
	free(data->y);
	free(data->optimal_iterate);

	return 0;
}

void show_data(data_t *data){
    printf("Num Samples: %d\n", data->num_samples);
    printf("Num Features: %d\n", data->num_features);

    printf("y:\n");
    for (int r = 0; r < data->num_samples; r++){
        printf("%f ", data->y[r]);
    }
    printf("\n");

    printf("beta:\n");
    for (int c = 0; c < data->num_features; c++){
        printf("%f ", data->optimal_iterate[c]);
    }
    printf("\n");

    printf("X:\n");
    for (int r = 0; r < data->num_samples; r++){
        for (int c = 0; c < data->num_features; c++){
            printf("%f ", data->X[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}

sparse_point_t * alloc_sparse_array(int len) {
	return (sparse_point_t *) malloc(len*sizeof(sparse_point_t));
}
