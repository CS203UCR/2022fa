#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int loop;

void* modifyloop(void *x)
{
  sleep(1);

  while(loop < 100)
  {
    loop++;
  }
  return NULL;
}

int main()
{
  pthread_t thread;
  loop = 1;
  
  pthread_create(&thread, NULL, modifyloop, NULL);
  while(loop<100)
  {
//    if(loop < 1000)
    fprintf(stderr,"%d ", loop);
  }
  pthread_join(thread, NULL);
  
  fprintf(stderr,"%4d finished\n", loop);
  return 0;
}
