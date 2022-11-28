#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#ifdef volatile
volatile int loop;

void* modifyloop(void *x)
{
  sleep(1);
  printf("Please input a number:\n");
  scanf("%d",&loop);
  return NULL;
}
#else
volatile int loop;

void* modifyloop(void *x)
{
  sleep(1);
  printf("Please input a number:\n");
  scanf("%d",&loop);
  return NULL;
}
#endif

int main()
{
  pthread_t thread;
  loop = 1;
  
  pthread_create(&thread, NULL, modifyloop, NULL);
  while(loop == 1)
  {
    continue;
  }
  pthread_join(thread, NULL); 
  fprintf(stderr,"User input: %d\n", loop);
  return 0;
}
