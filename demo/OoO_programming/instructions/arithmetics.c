#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main()
{
    int i,sum=0,data[131072],index=0;
    struct timeval time_start, time_end, program_start, program_end;
    for(i=0;i<131072;i++)
        data[i]=rand()%8738;
    gettimeofday(&time_start, NULL);
    for(i=0;i<1000000000;i++)
    {
        sum+=data[index];
        index = (data[index]*15)%131072;
    }
    gettimeofday(&time_end, NULL);
    printf("%lf seconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);

    gettimeofday(&time_start, NULL);
    for(i=0;i<1000000000;i++)
    {
        sum+=data[index];
        index = ((data[index] << 4) - data[index])&131071;
    }
    gettimeofday(&time_end, NULL);
    printf("%lf seconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);
        
    return 0;
}
