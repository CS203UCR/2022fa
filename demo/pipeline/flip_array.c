#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void (*swap)(int* a, int* b);
inline void regswap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void xorswap(int* a, int* b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}


int main(int argc, char **argv)
{
    unsigned array_size = 131072;
    int *data;
    struct timeval time_start, time_end;
    // command: swap array_size method
//    if(argc > 1)
//        array_size = atoi(argv[1]);
    data = (int *)malloc(sizeof(int)*array_size);
    for (unsigned i = 0; i < array_size; ++i)
        data[i] = rand();
   gettimeofday(&time_start, NULL);
//   for (unsigned i = 0; i < array_size; i+=2)
   for (unsigned i = 0; i < array_size; i++)
   {
        regswap(&data[i],&data[array_size-1-i]);
//        regswap(&data[i+1],&data[array_size-2-i]);
   }
   gettimeofday(&time_end, NULL);
   fprintf(stderr, "data[array_size/2] = %d\n", data[rand()%131072]);
   fprintf(stderr, "sorted %lf seconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);
   return 0;
}