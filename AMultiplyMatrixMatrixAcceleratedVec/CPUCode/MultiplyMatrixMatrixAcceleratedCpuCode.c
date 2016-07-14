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

#define VECTOR_SIZE MatMatMultiply_vectorSize

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

void alignMatrix(int n, int n_aligned, float *mat, float *mat_aligned)
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

void reverseAlignMatrix(int n, int n_aligned, float *mat_aligned, float *mat_final)
{
	for (int i = 0; i < n_aligned; i++) {
		for (int j = 0; j < n_aligned; j++){
			if (i <n && j<n) {
				mat_final[i * n + j] = mat_aligned[i * n_aligned + j];
			}
		}
	}
}

int main(int argc, char * argv[])
{
	parse_args(argc, argv);
	const int size = n * n;
	int n1 = calc_align(n, VECTOR_SIZE);
	int size1 = n1*n1;

	int dataSizeBytes = size * sizeof(float);
	int dataSizeBytes1 = size1 * sizeof(float);

	float *matA = malloc(dataSizeBytes);
	float *matB = malloc(dataSizeBytes);

	float *matAAligned = malloc(dataSizeBytes1);
	float *matBAligned = malloc(dataSizeBytes1);

	float *matATrans = malloc(dataSizeBytes1);
	float *matBTrans = malloc(dataSizeBytes1);
	float *mat_final = malloc(dataSizeBytes);

	float *vector;
	float *output = malloc(dataSizeBytes1);
	float *outputTrans = malloc(dataSizeBytes1);
	float *expected = malloc(dataSizeBytes);

	generateMatrix(n, matA, range);
	alignMatrix(n, n1, matA, matAAligned);

	generateMatrix(n, matB, range);
	alignMatrix(n, n1, matB, matBAligned);

	transform(n1, matAAligned, matATrans, VECTOR_SIZE);
	transpose(n1, matBAligned, matBTrans);

	timing_t timer;
	timer_start(&timer);
	for (int i=0; i<n1; i++) {
		vector = &matBTrans[n1*i];
		MatMatMultiply(n1*n1, n1, matATrans, vector, &output[n1*i]);
	}


	timer_stop(&timer);


	float sum = sumMat(size, output);
	printf("%d %f %ld %ld\n", n, sum, timer.realtime, timer.cputime);

	//A je potrebno transponirati zaradi vgradnje cevovoda
	//B je potrebno transponirati da lahko vsakič vzamemo en pokončni vektor
	//Rezultat je potrebno transponirati, ker ko dajemo vektorje skupaj pride ravno transponirano
	//Možno je brez vseh treh transponiranj

	int status = 0;

	if (trace == 1) {

		printf("\nMatrix A\n");
		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				printf("%f " , matA[i*n+j]);
			}
			printf("\n");
		}

		printf("\nMatrix B \n");
		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				printf("%f " , matB[i*n+j]);
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
		multiplyCPUMatrix(n, matA, matB, expected);
		transpose(n1, output, outputTrans);
		reverseAlignMatrix(n, n1, outputTrans, mat_final);
		int status = check(size, mat_final, expected);
		if (status) {
			printf("Test failed.\n");
			status = 1;
		}
		else
			printf("Test passed OK!\n");

	}

	free(matA);
	free(matB);
	free(matAAligned);
	free(matBAligned);
	free(matATrans);
	free(matBTrans);
	free(output);
	free(outputTrans);
	free(expected);
	free(mat_final);

	return status;
}
