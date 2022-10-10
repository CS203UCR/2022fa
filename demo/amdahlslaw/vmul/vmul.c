#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vmul.h"


int main(int argc, char **argv)
{
    double *a, *b;
    int i, iter=10, p=1, gpu = 0, array_size=16384;
    
    /* initialize a, b */
    if(argc >= 2)
        array_size = atoi(argv[1]);
    a = (double *)malloc(array_size*sizeof(double));
    for(i = 0; i < array_size; i++)
        a[i] = rand()/1000.0;
    b = (double *)malloc(array_size*sizeof(double));
    for(i = 0; i < array_size; i++)
        b[i] = rand()/1000.0;

    if(argc >= 3)
        p = atoi(argv[2]);
    if(argc >= 4)
        gpu = atoi(argv[3]);
    if(argc >= 5)
        iter = atoi(argv[4]);
    
    /* perform vector addition */
    if(gpu)
        vector_mul_double_cuda(a, b, array_size, p, iter);
    else
        vector_mul_double(a, b, array_size, p, iter);
    //for ( i = 0 ; i<10; i++)
    //    printf("check %lf ", a[rand()%array_size]);
    //printf("\n");
    for ( i = 0 ; i<4; i++)
        fprintf(stderr, "%lf,", perf_stats[i]);
    fprintf(stderr, "%lf\n", perf_stats[i]);
    return 0;
}
