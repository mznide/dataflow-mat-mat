#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include <sys/time.h>


void generate_matrix(int n, float *input, int range);
void print_matrix(int n, float *mat);

void transpose(int n, float *input, float *inputTransposed);
void multiply_CPU_matrix(int n, float *matrixA, float *matrixB, float *output);
int check(int size, float *output, float *expected);
float sum_mat(int size, float *mat);

void error(const int status, const char * fmt, ...);

void rand_init(const int seed);
short rand_sign();
int rand_int(const int range);
double rand_double(const int range);
double rand_signdouble(const int range);


#define PLAINTIMER

#ifdef __MACH__
#define PLAINTIMER
#endif

// timer_t is already used in time_h (Linux)
typedef struct timing_t {
#ifdef PLAINTIMER
    struct timeval realbegin, realend;
#else
    struct timespec realbegin, realend;
#endif
    clock_t cpubegin, cpuend;
    //
    clock_t realtime, cputime;  // in ms
} timing_t;

void timer_start(timing_t * t);
void timer_stop(timing_t * t);


#endif
