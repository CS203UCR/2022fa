#include <stdio.h>
#include <stdlib.h>

#ifdef NANO
#define mask_DPreFetch 0xA000

void DisablePrefetcher()
{
	unsigned long long r = 0;
	unsigned long long mask = ~mask_DPreFetch;

	__asm__ __volatile__ (
		"MRS %0, S3_1_C15_C2_0" : "=r" (r)
	);

	r &= mask;

	__asm__ __volatile__ (
		"MSR S3_1_C15_C2_0, %0" : : "r" (r)
	);
}
#endif

int main(int argc, char **argv)
{

  double *a, *b, *c, *d, *e, result;
  int i, j, k;
  int ARRAY_SIZE = 16384, RUN_ARRAY_SIZE=0;
  /* initialize a, b */
  if(argc >= 2)
    ARRAY_SIZE = atoi(argv[1]);
  if(argc >= 3)
    RUN_ARRAY_SIZE = atoi(argv[2]);
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
  /* Do matrix addition */
  //START
    for(i = 0; i < RUN_ARRAY_SIZE; i++)
    {
        e[i] = (a[i] * b[i] + c[i])/d[i];
#ifdef DUMP
        fprintf(stderr, "a[%d], %p\n",i,&a[i]);
        fprintf(stderr, "b[%d], %p\n",i,&b[i]);
        fprintf(stderr, "c[%d], %p\n",i,&c[i]);
        fprintf(stderr, "d[%d], %p\n",i,&d[i]);
        fprintf(stderr, "e[%d], %p\n",i,&e[i]);
#endif
    }
  //END
  printf("%lf\n",c[rand()%256]);
  return 0;
}
