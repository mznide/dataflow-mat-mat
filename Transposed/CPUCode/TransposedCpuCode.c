/**
 *
 * 	Summary:
 *     Naive solution for Matrix*Matrix multiplication.
 *     Repeats Matrix*vector multiplication to get Matrix*Matrix multiplication.
 *     Ensures inputs are transposed to match the order they are consumed in by the kernel.
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>

#include <MaxSLiCInterface.h>
#include "Maxfiles.h"
#include "common.h"

//TODO:  matrixes < 32

// show help
int help_flag = 0;

// number of rows / columns. Matrix size is n*n
int n = 32;

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

int main(int argc, char * argv[])
{
	parse_args(argc, argv);
	const int size = n * n;
	int data_size_bytes = size * sizeof(float);

	float *mat_a = malloc(data_size_bytes);
	float *mat_b = malloc(data_size_bytes);

	float *vector;
	float *output = malloc(data_size_bytes);
	float *expected = malloc(data_size_bytes);

	generate_matrix(n, mat_a, range);
	generate_matrix(n, mat_b, range);

	timing_t timer;
	timer_start(&timer);

	//A je potrebno transponirati zaradi vgradnje cevovoda
	//B je potrebno transponirati da lahko vsakič vzamemo en pokončni vektor
	//Rezultat je potrebno transponirati, ker ko dajemo vektorje skupaj pride ravno transponirano
	//Možno je brez vseh treh transponiranj

	for (int i=0; i<n; i++) {
		vector = &mat_a[n*i];
		MatMatMultiply(n*n, n, mat_b, vector, &output[n*i]);
	}

	timer_stop(&timer);
	float sum = sum_mat(size, output);
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
				printf("%f " , output[i*n+j]);
			}
			printf("\n");
		}
	}
	else if (trace == 2) {
		multiply_CPU_matrix(n, mat_a, mat_b, expected);
		int status = check(size, output, expected);
		//int status = check(size, output_trans, expected);
		if (status) {
			printf("Test failed.\n");
			status = 1;
		}
		else
			printf("Test passed OK!\n");

	}

	free(mat_a);
	free(mat_b);
	free(output);
	free(expected);

	return status;
}
