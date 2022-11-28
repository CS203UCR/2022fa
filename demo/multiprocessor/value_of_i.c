#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int i=47;

void* update_i(void *x)
{
  fprintf(stderr,"in_update_i i: %4d\n", i);
  i++;
  fprintf(stderr,"in_update_i i: %4d\n", i);
  return NULL;
}

int main()
{
  pthread_t thread;

  pthread_create(&thread, NULL, update_i, NULL);
  i++;
  pthread_join(thread, NULL);
  
  fprintf(stderr,"i: %4d\n", i);
  return 0;
}
