#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>
extern "C" {
#include "vmul.h"
}

__global__ void vector_mul_double_cuda_kernel(double *va, double *vb, int size, int grid_size, int iter);

extern void vector_mul_double_cuda(double *va, double *vb, unsigned long int size, int p, int iter)
{
    double *d_va, *d_vb;
    struct timeval time_start, time_end, program_start, program_end;
    gettimeofday(&time_start, NULL);
    cudaMalloc((void **) &d_va, sizeof(double)*size);
    cudaMalloc((void **) &d_vb, sizeof(double)*size);
    cudaMemcpy(d_va, va, sizeof(double)*size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_vb, vb, sizeof(double)*size, cudaMemcpyHostToDevice);
    gettimeofday(&time_end, NULL);
    perf_stats[HOST2GPU]=((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0;
    printf("memcpy (to device): %lf seconds\n",perf_stats[HOST2GPU]);
    gettimeofday(&time_start, NULL);

    unsigned int grid_size = 1024;
    unsigned int grid_cols = (size + grid_size - 1) / (p*1024);
    perf_stats[NUM_OF_THREADS] = grid_cols;
    dim3 dimGrid(grid_cols, 1);
    dim3 dimBlock(grid_size, 1);
	printf("Using GPU\n");
         // Launch kernel 
    vector_mul_double_cuda_kernel<<<(size + grid_size - 1) / (p*1024), 1024>>>(d_va, d_vb, size, p, iter);
    cudaThreadSynchronize();
    gettimeofday(&time_end, NULL);
    perf_stats[GPU_KERNEL] = ((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0;
    printf("kernel: %lf seconds\n",perf_stats[GPU_KERNEL]);
    gettimeofday(&time_start, NULL);
    cudaMemcpy(va, d_va, sizeof(double)*size,
    cudaMemcpyDeviceToHost);
    gettimeofday(&time_end, NULL);
    perf_stats[GPU2HOST] = ((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0;
    printf("memcpy (to host): %lf seconds\n",perf_stats[GPU2HOST]);
    cudaFree(d_va);
    cudaFree(d_vb);
	return;
}

__global__ void vector_mul_double_cuda_kernel(double *va, double *vb, int size, int grid_size, int iter) 
{
    unsigned int start_pos = blockIdx.x * blockDim.x + threadIdx.x;
    int i, j;
    for(i = start_pos*grid_size; i < (start_pos+1)*grid_size && i < size; i++)
        for (j = 0 ; j < iter; j++)
            va[i] = va[i]*vb[i];
}
