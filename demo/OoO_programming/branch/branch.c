#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
    int i, j, x, y, size;
    int *a;
    long long sum=0, sum_2=0;
    size = 100000000;
//    int sum=0;
    struct timeval time_start, time_end;
    x = atoi(argv[1]);
    y = atoi(argv[2]);
    if (argc == 4)
        size=atoi(argv[3]);
    a = (int *)malloc(sizeof(int)*size);
    for(i=0;i<size;i++)
    {
        a[i] = rand();
    }
    gettimeofday(&time_start, NULL);
    for(j=0;j<100;j++)
    for(i=0;i<size;i++)
    {
#ifdef BITWISE
         if (a[i] <= x || a[i] >= y)
             sum++;
#else
  #ifdef NO_BRANCH
             sum+=(a[i] < y & a[i] > x);
  #else
  #ifdef COND
         sum_2 = sum+1;
         sum = (a[i] < y && a[i] > x) ? sum_2: sum;
  #else // B
         if (a[i] < y && a[i] > x)
             sum++;
  #endif
  #endif
#endif
    }
    printf("sum: %lld\n", sum);
  gettimeofday(&time_end, NULL);
  printf("Takes %lf seconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);
    return 0;
}

/*
#ifdef twofirst
        if(a[i] % 2 == 0) sum_2+= a[i];
        if(a[i] % 4 == 0) sum_1+= a[i];
#else
        if(a[i] % 4 == 0) sum_1+= a[i];
        if(a[i] % 2 == 0) sum_2+= a[i];
#endif
*/