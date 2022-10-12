#include <stdio.h>
#include <stdlib.h>

int ARRAY_SIZE = 512;
void mm(int **a, int **b, int **c);

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
      a[i][j] = i+j;
  }
  b = (int **)malloc(ARRAY_SIZE*sizeof(int *));
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    b[i] = (int *)malloc(ARRAY_SIZE*sizeof(int));
    for(j = 0; j < ARRAY_SIZE; j++)
      b[i][j] = i+j;
  }
  c = (int **)malloc(ARRAY_SIZE*sizeof(int *));
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    c[i] = (int *)malloc(ARRAY_SIZE*sizeof(int));
    for(j = 0; j < ARRAY_SIZE; j++)
      c[i][j] = 0;
  }
  mm(a, b, c);
  /* Do matrix multiplication */
/*  for(i = 0; i < ARRAY_SIZE; i++)
  {
    for(j = 0; j < ARRAY_SIZE; j++)
    {
      for(k = 0; k < ARRAY_SIZE; k++)
      {
        c[i][j] += a[i][k]*b[k][j];
      }
    }
  }*/  
/*
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    for(j = 0; j < ARRAY_SIZE; j++)
    {
      printf("%d ",c[i][j]);
    }
    printf("\n");
  }
*/
  return 0;
}

void mm(int **a, int **b, int **c)
{
  int i,j,k;
  for(i = 0; i < ARRAY_SIZE; i++)
  {
    for(j = 0; j < ARRAY_SIZE; j++)
    {
      for(k = 0; k < ARRAY_SIZE; k++)
      {
        c[i][j] += a[i][k]*b[k][j];
      }
    }
  }
  return;
}
