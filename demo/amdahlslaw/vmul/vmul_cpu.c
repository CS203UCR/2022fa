#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vmul.h"
#include <sys/time.h>

void vector_mul_double(double *va, double *vb, unsigned long int size, int p, int iter)
{
    int i, j;
    struct timeval time_start, time_end, program_start, program_end;
    printf("Using CPU\n");
    gettimeofday(&time_start, NULL);
    for (i = 0 ; i < size; i++)
        for (j = 0 ; j < iter; j++)
            va[i] = va[i]*vb[i];
    gettimeofday(&time_end, NULL);
    perf_stats[CPU_TIME] = ((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0;
    printf("kernel: %lf seconds\n",perf_stats[CPU_TIME]);
    return;
}
