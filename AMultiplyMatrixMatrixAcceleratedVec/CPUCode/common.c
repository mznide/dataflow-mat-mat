#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#include "common.h"


void generateMatrix(int n, float *mat, int range)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			mat[i * n + j] = rand_signdouble(range);
}

void printMatrix(int n, float *mat){
	for (int i=0; i<n; i++){
		for (int j=0; j<n; j++){
			printf("%f " , mat[i*n+j]);
		}
		printf("\n");
	}
}

void transpose (int n, float *mat, float *matTransposed){
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			matTransposed[j + n * i] = mat[j * n + i];
}

void multiplyCPUMatrix(int n, float *matA, float *matB, float *res){
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			float sum = 0;
			for (int k = 0; k < n; k++) {
				sum +=  matA[i*n+k] * matB[k*n+j];
			}
			res[i*n+j] = sum;
	  }
	}
}

int check(int size, float *output, float *expected)
{
	int status = 0;
	for (int i = 0; i < size; i++) {
		if (output[i] != expected[i]) {
			fprintf(stderr, "[%d] error, output: %f != expected: %f\n",
				i, output[i], expected[i]);
			status = 1;
		}
	}
	return status;
}

float sumMat(int size, float *mat) {
	float sum = 0;
	for (int i = 0; i < size; i++)
		sum += mat[i];
	return sum;
}

void error(const int status, const char * fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, argp);
    fprintf(stderr, "\n");
    va_end(argp);
    exit(status);
}


void rand_init(const int seed) {
    srand(seed >= 0 ? (unsigned int) seed : (unsigned int) time(NULL));
}


short rand_sign() {
    return rand() < (RAND_MAX >> 1) ? 1 : -1;
}


int rand_int(const int range) {
    return rand() % range;
}


double rand_double(const int range) {
    double r = range * (double)rand() / (double)RAND_MAX;
    if (r == 0) return rand_double(range);
    return r;
}


double rand_signdouble(const int range) {
    return rand_double(range) *
    rand_sign();
}


void timer_start(timing_t * t) {
#ifdef PLAINTIMER
    gettimeofday(&t->realbegin, NULL);
#else
    clock_gettime(CLOCK_MONOTONIC, &t->realbegin);
#endif
    t->cpubegin = clock();
}


void timer_stop(timing_t * t) {
#ifdef PLAINTIMER
    gettimeofday(&t->realend, NULL);
#else
    clock_gettime(CLOCK_MONOTONIC, &t->realend);
#endif
    t->cpuend = clock();
    // calculate the difference in ms
    t->realtime = (t->realend.tv_sec - t->realbegin.tv_sec) * 1000;
#ifdef PLAINTIMER
    t->realtime += (t->realend.tv_usec - t->realbegin.tv_usec) / 1000;
#else
    t->realtime += (t->realend.tv_nsec - t->realbegin.tv_nsec) / 1000;
#endif
    t->cputime = (t->cpuend - t->cpubegin) * 1000 / CLOCKS_PER_SEC;
}
