#include <stdio.h>

int loop0(int *data, int size, int threshold, int reps)
{
    int i, j, sum = 0;
    for (j = 0; j < reps; ++j) {
        for (unsigned i = 0; i < size; ++i) {
            if (data[i] >= threshold)
                sum ++;
        }
    }
    return sum;
}
int loop1(int *a, int size)
{
    int i, sum = 0;
    for (i = 0; i < 10; i++)
    {
        sum += a[i];
    }
    return sum;
}

int loop2(int *a, int size)
{
    int i = 0;
    do {
        if( i % 2 != 0) // Branch X, taken if i % 2 == 0
           a[i] *= 2;
        a[i] += i;
    } while ( ++i < 100); // Branch Y
    return 0;
}

