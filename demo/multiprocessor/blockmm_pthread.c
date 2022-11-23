#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

int NUM_OF_THREADS=16;

void *threaded_blockmm(void *thread_id);
double **a, **b, **c;
int n=1;
  int ARRAY_SIZE=500;

int main(int argc, char **argv)
{
  int i, j, k;
  pthread_t *thread;
  int *tids;
  struct timeval time_start, time_end, program_start, program_end;
  /* initialize a, b */
  if(argc == 2)
    n = atoi(argv[1]);
  if(argc >= 3)
  {
    ARRAY_SIZE = atoi(argv[1]);
    n = atoi(argv[2]);
    NUM_OF_THREADS = atoi(argv[3]);
    
  }
  thread = (pthread_t *)malloc(sizeof(pthread_t)*NUM_OF_THREADS);
  tids = (int *)malloc(sizeof(pthread_t)*NUM_OF_THREADS);
  a = (double **)malloc(ARRAY_SIZE*sizeof(double *));
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    a[i] = (double *)malloc(ARRAY_SIZE*sizeof(double));
    for(j = 0; j < ARRAY_SIZE; j++)
      a[i][j] = i+j;
  }
  b = (double **)malloc(ARRAY_SIZE*sizeof(double *));
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    b[i] = (double *)malloc(ARRAY_SIZE*sizeof(double));
    for(j = 0; j < ARRAY_SIZE; j++)
      b[i][j] = i+j;
  }
  c = (double **)malloc(ARRAY_SIZE*sizeof(double *));
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    c[i] = (double *)malloc(ARRAY_SIZE*sizeof(double));
    for(j = 0; j < ARRAY_SIZE; j++)
      c[i][j] = 0;
  }
  /* Do matrix multiplication */
  gettimeofday(&time_start, NULL);
  for(i = 0 ; i < NUM_OF_THREADS ; i++)
  {
    tids[i] = i;
    pthread_create(&thread[i], NULL, threaded_blockmm, &tids[i]);
  }
  for(i = 0 ; i < NUM_OF_THREADS ; i++)
    pthread_join(thread[i], NULL);
  gettimeofday(&time_end, NULL);
  printf("Takes %lf seconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);

//  blockmm(a, b, c);
  
#ifdef OUTPUT
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    for(j = 0; j < ARRAY_SIZE; j++)
    {
      printf("%lf ",c[i][j]);
    }
    printf("\n");
  }
#endif
  return 0;
}

void *threaded_blockmm(void *thread_id)
{
  int i,j,k, ii, jj, kk;
  int chunk_size = ARRAY_SIZE/n;
  int tid = *(int *)thread_id;
//  fprintf(stderr,"TID: %d %d %d\n",tid,(ARRAY_SIZE/NUM_OF_THREADS)*(tid), (ARRAY_SIZE/NUM_OF_THREADS)*(tid+1));
  for(i = (ARRAY_SIZE/NUM_OF_THREADS)*(tid); i < (ARRAY_SIZE/NUM_OF_THREADS)*(tid+1); i+=chunk_size)
  {
    for(j = 0; j < ARRAY_SIZE; j+=chunk_size)
    {
      for(k = 0; k < ARRAY_SIZE; k+=chunk_size)
      {        
          for(ii = i; ii < i+chunk_size; ii++)
            for(jj = j; jj < j+chunk_size; jj++)
              for(kk = k; kk < k+chunk_size; kk++)
                c[ii][jj] += a[ii][kk]*b[kk][jj];
      }
    }
  }  
//  fprintf(stderr,"TID: %d finished\n",tid);
  return NULL;
}
