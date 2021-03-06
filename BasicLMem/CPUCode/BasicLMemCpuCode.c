/**
 * 	Summary:
 *
 *  i-th column of b is in fmem
    a is in lmem
	small loop because of s
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>

#include <MaxSLiCInterface.h>
#include "Maxfiles.h"
#include "common.h"
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
	if (argc <=1)
		return;

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

int calc_align(int n, int align) {
	return n / align * align + (n % align > 0) * align;
}

int main(int argc, char * argv[])
{
	parse_args(argc, argv);

	const int size = n * n;
	int data_size_bytes = calc_align(size, 384/sizeof(float)) * sizeof(float);

	float *mat_a = malloc(data_size_bytes);
	float *mat_b = malloc(data_size_bytes);
	float *vector;
	float *output = malloc(data_size_bytes);
	float *expected = malloc(data_size_bytes);

	generate_matrix(n, mat_a, range);
	generate_matrix(n, mat_b, range);

	timing_t timer;
	timer_start(&timer);

	float *mat_b_trans = malloc(data_size_bytes);
	transpose(n, mat_b, mat_b_trans);

	max_file_t * maxfile = MatMatMultiply_init();
	max_engine_t * engine = max_load(maxfile, "*");

	// write LP to LMem
	MatMatMultiply_writeLMem_actions_t writeact;
    writeact.param_address = 0;
    writeact.param_nbytes = data_size_bytes;

    writeact.instream_cpu_to_lmem = mat_a;
    MatMatMultiply_writeLMem_run(engine, &writeact);

    MatMatMultiply_actions_t actions;
	actions.param_matrixLength = calc_align(size, 384/sizeof(float));
	actions.param_n = n;

	for (int i=0; i<n; ++i) {
		vector = &mat_b_trans[n*i];
		actions.instream_vectorInput = vector;
		actions.outstream_output = &output[n*i];
		MatMatMultiply_run(engine, &actions);
	}

	max_unload(engine);

	float *output_trans = malloc(data_size_bytes);
	transpose(n, output, output_trans);
	timer_stop(&timer);

	float sum = sum_mat(size, output_trans);
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
		int status = check(size, output_trans, expected);
		if (status) {
			printf("Test failed.\n");
			status = 1;
		}
		else
			printf("Test passed OK!\n");

	}

	free(mat_a);
	free(mat_b);
	free(mat_b_trans);
	free(output);
	free(expected);

	return status;
}
