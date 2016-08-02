#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#include "common.h"


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
    // cpu time
    t->cpubegin = clock();
    // real time
#ifdef PLAINTIMER
    gettimeofday(&t->realbegin, NULL);
#else
    clock_gettime(CLOCK_MONOTONIC, &t->realbegin);
#endif
}


void timer_stop(timing_t * t) {
    // cpu time
    t->cpuend = clock();
    t->cputime = (t->cpuend - t->cpubegin) * 1000 / CLOCKS_PER_SEC;
    // real time
#ifdef PLAINTIMER
    gettimeofday(&t->realend, NULL);
#else
    clock_gettime(CLOCK_MONOTONIC, &t->realend);
#endif
    t->realtime = (t->realend.tv_sec - t->realbegin.tv_sec) * 1000;
#ifdef PLAINTIMER
    t->realtime += (t->realend.tv_usec - t->realbegin.tv_usec) / 1000;
#else
    t->realtime += (t->realend.tv_nsec - t->realbegin.tv_nsec) / 1000;
#endif
}
