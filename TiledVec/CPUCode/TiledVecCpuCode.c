/**
 * 	Summary:
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>

#include <MaxSLiCInterface.h>
#include "Maxfiles.h"
#include "common.h"

#define VECTOR_SIZE MatMatMultiply_vectorSize

//TODO:  matrixes < 32

// show help
int help_flag = 0;

// number of rows / columns. Matrix size is n*n
int n = 32;
#define C MatMatMultiply_C

/* tracing
	0 - prints only n, sum of result, realtime, cputime
	1 - prints input, output, final result
	2 - tests correctness of result and prints if test passed
*/
int trace = 0;

// elements of matrix are in -range  to +range interval
float range = 100.0;

void help(const char * cmd) {
    printf("Usage: %s [filename]\n", cmd);
    printf("\nOptions:\n");
    printf("  -h, --help\n\tPrint short help\n");
    printf("  -n, --size\n\tSize n of matrix\n");
    printf("  -r, --range\n\tRange of elements\n");
    printf("  -t, --trace\n\tTrace level: 0,1,2\n");

};

struct option options[] = {
	{"help",	required_argument, 0, 'h'},
	{"size",	required_argument, 0, 'n'},
	{"trace",	required_argument, 0, 't'},
	{"range",	required_argument, 0, 'r'},
	{0,0,0,0}
};

#define SHORTOPT "hn:t:r:"

void parse_args(int argc, char * argv[]) {
	while (1) {
		int option_index = 0;
		int opt = getopt_long(argc, argv, SHORTOPT, options, &option_index);

		if (opt == -1) break;

		switch (opt) {
			case 'h':
				help_flag = 1;
				break;
			case 'n':
				n = atoi(optarg);
				break;
			case 't':
				trace = atoi(optarg);
				break;
			case 'r':
				range = atoi(optarg);
				break;
			case '?':
				error(1, "Invalid1 option '%c'", optopt);
			default:
				abort();
		}
	}
	if (help_flag) {
		help(argv[0]);
		exit(0);
	}
}


//transforms matrix for vectorized multiplication
void transform (int n, float *input, float *matrixTransformed, int vectorSize){
	for (int v = 0; v < n; v=v+vectorSize) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < vectorSize; j++) {
				matrixTransformed[i*vectorSize +j + v*n] = input[j+i*n+v];

			}
		}
	}
}

int calc_align(int n, int align) {
	return n / align * align + (n % align > 0) * align;
}

void align_matrix(int n, int n_aligned, float *mat, float *mat_aligned)
{
	for (int i = 0; i < n_aligned; i++) {
		for (int j = 0; j < n_aligned; j++){
			if (i <n && j<n) {
				mat_aligned[i * n_aligned + j] = mat[i * n + j];
			}
			else {
				mat_aligned[i * n_aligned + j] = 0;
			}
		}
	}
}


void transform_together(int n, float *inputMatrix, float *matrixTransformed, int vectorSize, int C1){
	int count = 0;
	for (int yy = 0; yy < n; yy += C1) {
		for ( int x = 0; x < n/vectorSize; ++x) {
			for ( int y = yy; y < yy + C1; ++y) {
				for (int v = 0; v < vectorSize; v++){
					matrixTransformed[count] = inputMatrix[y * n + x*vectorSize+v];
					count++;
					//printf("yy %d x %d y %d v %d \n", yy, x,y,v);
				}
			}
		}
	}
	//printf("n %d count %d \n", n, count);
}

void reverse_align_matrix(int n, int n_aligned, float *mat_aligned, float *mat_final)
{
	for (int i = 0; i < n_aligned; i++) {
		for (int j = 0; j < n_aligned; j++){
			if (i <n && j<n) {
				mat_final[i * n + j] = mat_aligned[i * n_aligned + j];
			}
		}
	}
}

int gcd(int a, int b) {
	if (b == 0) {
		return a;
	}
	else {
		return gcd(b, a % b);
	}
}

int main(int argc, char * argv[])
{
	parse_args(argc, argv);
	const int size = n * n;

	int hcf = gcd(VECTOR_SIZE, C);
	int lcm = (VECTOR_SIZE*C)/hcf;
	int n1 = calc_align(n, lcm);
	int size1 = n1*n1;

	int dataSizeBytes = size * sizeof(float);
	int dataSizeBytes1 = size1 * sizeof(float);

	float *mat_a = malloc(dataSizeBytes);
	float *mat_b = malloc(dataSizeBytes);

	float *mat_a_aligned = malloc(dataSizeBytes1);
	float *mat_b_aligned = malloc(dataSizeBytes1);

	float *mat_a_trans = malloc(dataSizeBytes1);
	float *mat_b_trans = malloc(dataSizeBytes1);
	float *mat_final = malloc(dataSizeBytes);


	float *vector;
	float *output = malloc(dataSizeBytes1);
	float *output_trans = malloc(dataSizeBytes1);
	float *expected = malloc(dataSizeBytes);

	generate_matrix(n, mat_a, range);
	generate_matrix(n, mat_b, range);

	timing_t timer;
	timer_start(&timer);

	align_matrix(n, n1, mat_b, mat_b_aligned);
	align_matrix(n, n1, mat_a, mat_a_aligned);

	transform_together(n1, mat_a_aligned, mat_a_trans, VECTOR_SIZE, C);
	transpose(n1, mat_b_aligned, mat_b_trans);

	for (int i=0; i<n1; i++) {
		vector = &mat_b_trans[n1*i];
		MatMatMultiply(n1*n1, n1, mat_a_trans, vector, &output[n1*i]);
	}

	transpose(n1, output, output_trans);
	reverse_align_matrix(n, n1, output_trans, mat_final);

	timer_stop(&timer);


	float sum = sum_mat(size, mat_final);
	printf("%d %f %ld %ld\n", n, sum, timer.realtime, timer.cputime);


	int status = 0;

	if (trace == 1) {
		printf("\nMatrix A\n");
		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				printf("%f " , mat_a[i*n+j]);
			}
			printf("\n");
		}

		printf("\nMatrix B \n");
		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				printf("%f " , mat_b[i*n+j]);
			}
			printf("\n");
		}

		printf("\n\nResult\n");
		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				printf("%f " , mat_final[i*n+j]);
			}
			printf("\n");
		}
	}
	else if (trace == 2) {
		multiply_CPU_matrix(n, mat_a, mat_b, expected);
		transpose(n, output, output_trans);
		int status = check(size, mat_final, expected);
		if (status) {
			printf("Test failed.\n");
			status = 1;
		}
		else
			printf("Test passed OK!\n");

	}

	free(mat_a);
	free(mat_b);
	free(mat_a_trans);
	free(mat_b_trans);
	free(mat_a_aligned);
	free(mat_b_aligned);
	free(output);
	free(output_trans);
	free(mat_final);
	free(expected);

	return status;
}
