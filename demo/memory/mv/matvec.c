#include <stdio.h>
#include <stdlib.h>

int ARRAY_SIZE = 8192;

int main(int argc, char **argv)
{
  double **a, *b, *c, result;
  int i,j,dump=0;
  /* initialize a, b */
  if(argc >= 2)
    ARRAY_SIZE = atoi(argv[1]);
  if(argc >= 3)
    dump = atoi(argv[2]);
  a = (double **)malloc(ARRAY_SIZE*sizeof(double *));
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    a[i] = (double *)malloc(ARRAY_SIZE*sizeof(double));
    for(j = 0; j < ARRAY_SIZE; j++)
      a[i][j] = (rand()%100)+1;
  }

  b = (double *)malloc(ARRAY_SIZE*sizeof(double));
  for(i = 0; i < ARRAY_SIZE; i++)
      b[i] = (rand()%100)+1;

  c = (double *)malloc(ARRAY_SIZE*sizeof(double));

  for(i = 0; i < ARRAY_SIZE; i++)
      c[i] = 0.0;
  /* Do matrix addition */
  //START
    for(i = 0; i < ARRAY_SIZE; i++)
    {
      result = 0.0;    
      for(j = 0; j < ARRAY_SIZE; j++)
      {
        result += a[i][j]*b[j];
        if(dump)
            printf("&a[%d][%d], %p\n&b[%d], %p\n", i, j, &a[i][j], j, &b[j]);
      }
      c[i] = result;
      if(dump)
          printf("&c[%d], %p\n", i, &c[i]);
    }
  //END
  printf("%lf\n",c[rand()%ARRAY_SIZE]);
  return 0;
}
