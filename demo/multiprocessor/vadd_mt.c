#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <sys/time.h>
#include <pthread.h>
#include "perfstats.h"

int ARRAY_SIZE = 16384*16384;
void *threaded_vadd(void *thread_id);

  float *a, *b, *c;
  int NUM_OF_THREADS;

int main(int argc, char **argv)
{
  int i, j, k;
    struct timeval time_start, time_end, program_start, program_end;
        char preamble[1024];
        char epilogue[1024];
        char header[1024];
        char stat_file[] = "stats.csv";
  /* initialize a, b */
  pthread_t *thread;
  int *tids;
  NUM_OF_THREADS = 4;
  if(argc >= 2)
  {
    ARRAY_SIZE = atoi(argv[1]);
    NUM_OF_THREADS = atoi(argv[2]);
  }
  thread = (pthread_t *)malloc(sizeof(pthread_t)*NUM_OF_THREADS);
  tids = (int *)malloc(sizeof(pthread_t)*NUM_OF_THREADS);

  a = (float *)malloc(ARRAY_SIZE*sizeof(float));
  for(i = 0; i < ARRAY_SIZE; i++)
      a[i] = (rand()%100)+1;
  b = (float *)malloc(ARRAY_SIZE*sizeof(float));
  for(i = 0; i < ARRAY_SIZE; i++)
    b[i] = (rand()%100)+1;
  c = (float *)malloc(ARRAY_SIZE*sizeof(float));
  for(i = 0; i < ARRAY_SIZE; i++)
    c[i] = 0;
//  mm(a, b, c);
  /* Do matrix addition */
  gettimeofday(&time_start, NULL);
    perfstats_init();
    perfstats_enable(1);
  for(i = 0 ; i < NUM_OF_THREADS ; i++)
  {
    tids[i] = i;
    pthread_create(&thread[i], NULL, threaded_vadd, &tids[i]);
  }
  for(i = 0 ; i < NUM_OF_THREADS ; i++)
    pthread_join(thread[i], NULL);

    perfstats_disable(1);
    sprintf(epilogue,"\n");
    sprintf(preamble,"");
    perfstats_print(preamble, stat_file, epilogue);
    perfstats_deinit();
  gettimeofday(&time_end, NULL);
  printf("Takes %lf seconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);
        fprintf(stderr, "%f\t", c[rand()%ARRAY_SIZE]);
#ifdef VERBOSE
    for(i = 0; i < ARRAY_SIZE; i++)
    {
        fprintf(stderr, "%f\t", c[i]);
    }
#endif
  return 0;
}

void *threaded_vadd(void *thread_id)
{
  int tid = *(int *)thread_id;
  int i = tid * 4;
  //int ops=0;
  #ifdef CHUNK
  for(i = tid*(ARRAY_SIZE/NUM_OF_THREADS); i < (tid+1)*(ARRAY_SIZE/NUM_OF_THREADS); i++)
  {
      c[i] = a[i] + b[i];
  }
  #else // interleaving
  for(i = tid; i < ARRAY_SIZE; i+=NUM_OF_THREADS)
  {
      c[i] = a[i] + b[i];
  }
  #endif
//        fprintf(stderr, "%d\t", ops);
  return NULL;
}
