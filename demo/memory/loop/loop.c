#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int ARRAY_SIZE = 500;
void loop(double *a, double *b, double *c, double *d, double *e);

int main(int argc, char **argv)
{
  double *a, *b, *c, *d, *e;
  int i, j, k;
    struct timeval time_start, time_end, program_start, program_end;
  /* initialize a, b */
  if(argc >= 2)
    ARRAY_SIZE = atoi(argv[1]);
//  if(argc >= 3)
//    RUN_ARRAY_SIZE = atoi(argv[2]);
  a = (double *)valloc(ARRAY_SIZE*5*sizeof(double));
  for(i = 0; i < ARRAY_SIZE; i++)
      a[i] = (rand()%100)+1;
  b = &a[ARRAY_SIZE];
  for(i = 0; i < ARRAY_SIZE; i++)
      b[i] = (rand()%100)+1;
  c = &b[ARRAY_SIZE];
  for(i = 0; i < ARRAY_SIZE; i++)
      c[i] = (rand()%100)+1;
  d = &c[ARRAY_SIZE];
  for(i = 0; i < ARRAY_SIZE; i++)
      d[i] = (rand()%100)+1;
  e = &d[ARRAY_SIZE];
  for(i = 0; i < ARRAY_SIZE; i++)
      e[i] = (rand()%100)+1;
    gettimeofday(&time_start, NULL);
    loop(a,b,c,d,e);
    gettimeofday(&time_end, NULL);
    printf("random in e:%lf %lf seconds\n",e[rand()%ARRAY_SIZE], ((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);

  return 0;
}

void loop(double *a, double *b, double *c, double *d, double *e)
{
  int i,j,k;
  #ifdef A
  for(i = 0; i < ARRAY_SIZE; i++)
  {
     e[i] = (a[i] * b[i] + c[i])/d[i];
  }
  #else
  for(i = 0; i < ARRAY_SIZE; i++)
      e[i] = a[i] * b[i] + c[i];
  for(i = 0; i < ARRAY_SIZE; i++)
      e[i] /= d[i];
  #endif
  return;
}
