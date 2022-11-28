#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* modifya(void *z);
void* modifyb(void *z);
volatile int a,b;
volatile int x,y;
volatile int f;
volatile int go;

#ifndef MFENCE
void* modifya(void *z)
{
  while(!go);
  a=1;
  x=b;
  return NULL;
}
void* modifyb(void *z)
{
  while(!go);
  b=1;
  y=a;
  return NULL;
}
#endif
int main()
{
  int i;
  pthread_t thread[2];
  pthread_create(&thread[0], NULL, modifya, NULL);
  pthread_create(&thread[1], NULL, modifyb, NULL);
  go = 1;
  pthread_join(thread[0], NULL);
  pthread_join(thread[1], NULL);
  fprintf(stderr,"After join: (%d, %d)\n",x,y);
  return 0;
}
#ifdef MFENCE
#define _update_var(_v, _x) { _v = (_x); asm("mfence"); }
void* modifya(void *z)
{
//  a=1;
  while(!go);
  _update_var(a,1);
  x=b;
  return NULL;
}
void* modifyb(void *z)
{
//  b=1;
  while(!go);
  _update_var(b,1);
  y=a;
  return NULL;
}
#endif