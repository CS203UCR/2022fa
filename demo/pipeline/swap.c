#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void (*swap)(int* a, int* b);
void regswap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void xorswap(int* a, int* b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition (int arr[], int low, int high)
{
    int pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element
 
    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
        {
            i++;    // increment index of smaller element
            if(i!=j)
            swap(&arr[i], &arr[j]);
        }
    }
            if(i+1 != high)
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
 
/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(arr, low, high);
 
        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
 

int main(int argc, char **argv)
{
    unsigned array_size = 131072;
    int *data;
    struct timeval time_start, time_end;
    // command: swap array_size method
    if(argc > 1)
        array_size = atoi(argv[1]);
    data = (int *)malloc(sizeof(int)*array_size);
    if(argc > 2 && atoi(argv[2])==1)
        swap = xorswap;
    else
        swap = regswap;
    for (unsigned i = 0; i < array_size; ++i)
        data[i] = rand();
   gettimeofday(&time_start, NULL);
   quickSort(data, 0, array_size-1);
/*   for (unsigned i = 0; i < array_size; ++i)
        swap(&data[i],&data[array_size-1-i]);*/
   gettimeofday(&time_end, NULL);
   fprintf(stderr, "data[rand()] = %d\n", data[rand()%array_size]);
   fprintf(stderr, "sorted %lf seconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);
   return 0;
}