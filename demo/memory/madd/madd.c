#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ARRAY_SIZE = 500;

int main(int argc, char **argv)
{
  int **a, **b, **c, i, j, k;
  /* initialize a, b */
  if(argc >= 2)
    ARRAY_SIZE = atoi(argv[1]);
  a = (int **)malloc(ARRAY_SIZE*sizeof(int *));
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    a[i] = (int *)malloc(ARRAY_SIZE*sizeof(int));
    for(j = 0; j < ARRAY_SIZE; j++)
      a[i][j] = (rand()%100)+1;
  }
  b = (int **)malloc(ARRAY_SIZE*sizeof(int *));
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    b[i] = (int *)malloc(ARRAY_SIZE*sizeof(int));
    for(j = 0; j < ARRAY_SIZE; j++)
      b[i][j] = (rand()%100)+1;
  }
  c = (int **)malloc(ARRAY_SIZE*sizeof(int *));
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    c[i] = (int *)malloc(ARRAY_SIZE*sizeof(int));
    for(j = 0; j < ARRAY_SIZE; j++)
      c[i][j] = 0;
  }
//  mm(a, b, c);
  /* Do matrix addition */
  if(strcmp("row", argv[2])==0)
  {
    for(i = 0; i < ARRAY_SIZE; i++)
    {
      for(j = 0; j < ARRAY_SIZE; j++)
      {
        c[i][j] = a[i][j]+b[i][j];
      }
    }
  }
  else  
    for(j = 0; j < ARRAY_SIZE; j++)
    {
      for(i = 0; i < ARRAY_SIZE; i++)
      {
        c[i][j] = a[i][j]+b[i][j];
      }
    }

  return 0;
}
