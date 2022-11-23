#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <sys/time.h>
#include <pthread.h>

int ARRAY_SIZE = 16384*16384;
void *threaded_vadd(void *thread_id);

int main(int argc, char **argv)
{
  float *a, *b, *c;
  int i, j, k;
    struct timeval time_start, time_end, program_start, program_end;
  /* initialize a, b */
  __m128 va, vb, vt;
  pthread_t *thread;
  int *tids;
  if(argc >= 2)
    ARRAY_SIZE = atoi(argv[1]);
  NUM_OF_THREADS = ARRAY_SIZE/4;
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
  for(i = 0 ; i < NUM_OF_THREADS ; i++)
  {
    tids[i] = i;
    pthread_create(&thread[i], NULL, threaded_vadd, &tids[i]);
  }
  for(i = 0 ; i < NUM_OF_THREADS ; i++)
    pthread_join(thread[i], NULL);
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
        va = _mm_load_ps(&a[i]);
        vb = _mm_load_ps(&b[i]);
        vt = _mm_add_ps(va, vb);
        _mm_store_ps(&c[i],vt);
  return NULL;
}
