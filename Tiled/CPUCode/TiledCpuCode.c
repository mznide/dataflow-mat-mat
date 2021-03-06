/**
 * 	Summary:
 *     CPU code for Matrix*vector multiplication.
 *     Ensures inputs are transposed to match the order they are consumed in by the kernel.
 * 	   # Matrix is read as a stream (transposed matrix)
 *	   # x is read as a stream (every n ticks)
 *	   # big loop (size=n)
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <MaxSLiCInterface.h>
#include "Maxfiles.h"
#include "common.h"

#define C MatMatMultiply_C

void help(const char * cmd) {
    printf("Usage: %s [filename]\n", cmd);
    printf("\nOptions:\n");
    printf("  -h, --help\n\tPrint short help\n");
    printf("  -n, --size\n\tSize n of matrix\n");
    printf("  -r, --range\n\tRange of elements\n");
    printf("  -t, --trace\n\tTrace level: 0,1,2\n");

};

// show help
int help_flag = 0;

// number of rows / columns. Matrix size is n*n
int n = 64;

/* tracing
	0 - prints only n, sum of result, realtime, cputime
	1 - prints input, output, final result
	2 - tests correctness of result and prints if test passed
*/
int trace = 0;



// elements of matrix are in -range  to +range interval
float range = 100.0;

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

void transform_C(int n, float *mat, float *matrixTransformed, int C1){
	int count = 0;
	for (int yy = 0; yy < n; yy += C1) {
		for ( int x = 0; x < n; ++x) {
			for ( int y = yy; y < yy + C1; ++y) {
				matrixTransformed[count] = mat[y * n + x];
				count++;
			}
		}
	}
}



int main(int argc, char * argv[])
{
	if (argc > 1){
		parse_args(argc, argv);
	}

	const int size = n * n;
	int dataSizeBytes = size * sizeof(float);

	float *mat_a = malloc(dataSizeBytes);
	float *mat_b = malloc(dataSizeBytes);
	float *output = malloc(dataSizeBytes);
	float *expected = malloc(dataSizeBytes);
		float *vector;

	generate_matrix(n, mat_a, range);
	generate_matrix(n, mat_b, range);

	timing_t timer1;
	timer_start(&timer1);

	timing_t timer2;
	timer_start(&timer2);

	float *output_trans = malloc(dataSizeBytes);
	float *mat_a_trans = malloc(dataSizeBytes);
	float *mat_b_trans = malloc(dataSizeBytes);

	transform_C(n, mat_a, mat_a_trans, C);
	transpose(n, mat_b, mat_b_trans);
	timer_stop(&timer2);
	for (int i=0; i<n; i++) {
		vector = &mat_b_trans[n*i];
		MatMatMultiply(n*n, n, mat_a_trans, vector, &output[n*i]);
	}

	timer_stop(&timer1);
	float sum = sum_mat(n, output);

	printf("%d %f %ld %ld %ld %ld\n", n, sum, timer1.realtime, timer1.cputime, timer2.realtime, timer2.cputime);


	if (trace == 1) {
		printf("\nInput matrix A\n");

		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				printf("%f " , mat_a[i*n+j]);
			}
			printf("\n");
		}
		printf("\nInput vector \n");

		printf("\nInput matrix b\n");

		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				printf("%f " , mat_b[i*n+j]);
			}
			printf("\n");
		}
		printf("\n\nResult\n");
		for (int i=0; i<n; i++){
			printf("%f " , output[i]);
		}
		printf("\n");
	}
	else if (trace == 2) {

		multiply_CPU_matrix(n, mat_a, mat_b, expected);
		transpose(n, output, output_trans);
		int status = check(n*n, output_trans, expected);
		if (status)
			printf("Test failed.\n");
		else
			printf("Test passed OK!\n");
		return status;
	}


	free(mat_a);
	free(mat_b);

	free(output);
	free(expected);
	free(mat_a_trans);
	free(mat_b_trans);

	return 0;
}
