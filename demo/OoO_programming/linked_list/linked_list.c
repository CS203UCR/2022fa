#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "perfstats.h"
struct node
{
    int value;
    int linked;
    struct node *next;
};

int depth_of_list(struct node *start)
{
    int number_of_nodes = 0;
    struct node *node = start;
    while(node)
    {
        node = node->next;
        number_of_nodes++;
    }
    return number_of_nodes;
}

int sum_of_array(struct node *start, int size)
{
    int number_of_nodes = 0, i;
    unsigned long sum=0;
    struct node *node = start;
    for(i=0;i<size;i++)
    {
        if(node[i].next)
            sum+=node[i].value;
    }
    return sum;
}

int main(int argc, char **argv)
{
    unsigned array_size = 131072, depth;
    struct node *data;
    struct timeval time_start, time_end;
        char preamble[1024];
        char epilogue[1024];
        char header[1024];
        char stat_file[] = "stats.csv";
    int list = 0, toLink;
    if(argc >= 2)
        array_size = atoi(argv[1]);
    if(argc >= 3)
        list = atoi(argv[2]);
    data = (struct node*)calloc(array_size, sizeof(struct node));
    for (unsigned i = 0; i < array_size; ++i)
        data[i].value = rand();
    for (unsigned i = 0; i < array_size-1; ++i)
    {
        do {
        toLink = rand() & (array_size-1);}
        while(data[toLink].linked != 0);
        data[i].next = &data[toLink];
        data[toLink].linked = 1;
    }
   gettimeofday(&time_start, NULL);
    perfstats_init();
    perfstats_enable(1);
    if(list)
        depth = depth_of_list(data);
    else
        depth = sum_of_array(data, array_size);
        
   gettimeofday(&time_end, NULL);
    perfstats_disable(1);
    sprintf(epilogue,"\n");
    sprintf(preamble,"");
    perfstats_print(preamble, stat_file, epilogue);
    perfstats_deinit();
   fprintf(stderr, "depth: %lu\n", depth);
//   fprintf(stderr, "sorted %lf seconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);
   return 0;
}

