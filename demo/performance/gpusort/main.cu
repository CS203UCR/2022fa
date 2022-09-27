#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "helper_cuda.h"
#include "helper_timer.h"
#include <iostream>
#include <sys/time.h>
#include <cuda.h>

using namespace std; 

#define SIZE	(1 << 22)
// Number of tests to average over
#define TEST	1
// The timers for the different parts of the algo
StopWatchInterface  *uploadTimer, *downloadTimer, *bucketTimer, 
			 *mergeTimer, *totalTimer, *cpuTimer; 

// Compare method for CPU sort
inline int compare(const void *a, const void *b) {
	if(*((float *)a) < *((float *)b)) return -1; 
	else if(*((float *)a) > *((float *)b)) return 1; 
	else return 0; 
}

__global__ void bitonic_sort_step(float *dev_values, int j, int k)
{
  unsigned int i, ixj; /* Sorting partners: i and ixj */
  i = threadIdx.x + blockDim.x * blockIdx.x;
  ixj = i^j;

  /* The threads with the lowest ids sort the array. */
  if ((ixj)>i) {
    if ((i&k)==0) {
      /* Sort ascending */
      if (dev_values[i]>dev_values[ixj]) {
        /* exchange(i,ixj); */
        float temp = dev_values[i];
        dev_values[i] = dev_values[ixj];
        dev_values[ixj] = temp;
      }
    }
    if ((i&k)!=0) {
      /* Sort descending */
      if (dev_values[i]<dev_values[ixj]) {
        /* exchange(i,ixj); */
        float temp = dev_values[i];
        dev_values[i] = dev_values[ixj];
        dev_values[ixj] = temp;
      }
    }
  }
}

/**
 * Inplace bitonic sort using CUDA.
 */
//#define BLOCKS 32768 // 2^15
#define THREADS_PER_BLOCK 1024 //

void bitonic_sort(float *values, float *out_values, int num_of_elements)
{
  float *dev_values;
  size_t size = num_of_elements * sizeof(float);
  int BLOCKS = num_of_elements/THREADS_PER_BLOCK; 

  cudaMalloc((void**) &dev_values, size);
  sdkStartTimer(&uploadTimer);
  cudaMemcpy(dev_values, values, size, cudaMemcpyHostToDevice);
  sdkStopTimer(&uploadTimer); 
  dim3 blocks(BLOCKS,1);    /* Number of blocks   */
  dim3 threads(THREADS_PER_BLOCK,1);  /* Number of threads  */

  sdkStartTimer(&totalTimer); 
  int j, k;
  /* Major step */
  for (k = 2; k <= num_of_elements; k <<= 1) {
    /* Minor step */
    for (j=k>>1; j>0; j=j>>1) {
      bitonic_sort_step<<<blocks, threads>>>(dev_values, j, k);
    }
  }
		cudaThreadSynchronize(); 
  sdkStopTimer(&totalTimer); 
  sdkStartTimer(&downloadTimer);
  cudaMemcpy((void *)out_values, (void *)dev_values,  num_of_elements*sizeof(float), cudaMemcpyDeviceToHost);
//		checkCudaErrors(cudaMemcpy((void *)out_values, (void *)dev_values, num_of_elements*sizeof(float), cudaMemcpyDeviceToHost));
  sdkStopTimer(&downloadTimer); 
  cudaFree(dev_values);
}

 #define N		THREADS_PER_BLOCK
 // Kernel function
 __global__ static void quicksort(float* values) {
 #define MAX_LEVELS	300

	int pivot, L, R;
	int idx =  threadIdx.x + blockIdx.x * blockDim.x;
	int start[MAX_LEVELS];
	int end[MAX_LEVELS];

	start[idx] = idx;
	end[idx] = N - 1;
	while (idx >= 0) {
		L = start[idx];
		R = end[idx];
		if (L < R) {
			pivot = values[L];
			while (L < R) {
				while (values[R] >= pivot && L < R)
					R--;
				if(L < R)
					values[L++] = values[R];
				while (values[L] < pivot && L < R)
					L++;
				if (L < R)
					values[R--] = values[L];
			}
			values[L] = pivot;
			start[idx + 1] = L + 1;
			end[idx + 1] = end[idx];
			end[idx++] = L;
			if (end[idx] - start[idx] > end[idx - 1] - start[idx - 1]) {
	                        // swap start[idx] and start[idx-1]
        	                int tmp = start[idx];
                	        start[idx] = start[idx - 1];
                        	start[idx - 1] = tmp;

	                        // swap end[idx] and end[idx-1]
        	                tmp = end[idx];
                	        end[idx] = end[idx - 1];
                        	end[idx - 1] = tmp;
	                }

		}
		else
			idx--;
	}
}


typedef struct vars{
  int l;
  int r;
  int leq;
} vars;

/***************************** GPU IMPLEMENTATION ****************************/

/* gpuPartitionSwap
 *
 * This kernel function is called recursively by the host. Its purpose is to, 
 * given a pivot value, partition and swap items in the section of the input
 * array bounded by the l & r indices, then store the pivot in the correct
 * location.
 *
 * Parameters:
 * input: The unsorted (or partially sorted) input data
 * output: The aptly named output parameter, it is the same as input, but all
 *         floating points within (l,r) have been partitioned and swapped.
 * endpts: This is a custom data struct meant to 
 *         a) hold a counter variable in global memory
 *         b) pass the l' and r' parameters back to the host to the left and
 *            right of the positioned pivot item.
 * pivot: This is the pivot value, about which all items in (l,r) are being
 *        swapped.
 * l: the left index bound on input & output
 * r: the right index bound on input & output
 * d_leq: an array of offset values, storedin global device memory
 * nBlocks: The total number of blocks, to be used to determine the location
 *          of insertion of the pivot.
 *
 */
__global__ void gpuPartitionSwap(float * input, float * output, vars * endpts, 
				 float pivot, int l, int r, 
                                 int d_leq[], 
				 int d_gt[], int *d_leq_val, int *d_gt_val, 
                                 int nBlocks)
{
  //copy a section of the input into shared memory
  __shared__ float bInput[THREADS_PER_BLOCK];
  __syncthreads();
  int idx = l + blockIdx.x*THREADS_PER_BLOCK + threadIdx.x;
  __shared__ int lThisBlock, rThisBlock;
  __shared__ int lOffset, rOffset;

  if(threadIdx.x == 0){
    d_leq[blockIdx.x] = 0;
    d_gt[blockIdx.x] = 0;
    *d_leq_val = 0;
    *d_gt_val = 0;
    //cuPrintf("STARTING NEXT SET!  l: %d, r: %d\n", l, r);
  }
  __syncthreads();

  if(idx <= (r - 1)){
    bInput[threadIdx.x] = input[idx];

    //make comparison against the pivot, setting 'status' and updating the counter (if necessary)
    if( bInput[threadIdx.x] <= pivot ){
        //cuPrintf("input:  %f < pivot: %f\n", bInput[threadIdx.x], pivot);
        atomicAdd( &(d_leq[blockIdx.x]), 1);
    } else {
        //cuPrintf("input:  %f > pivot: %f\n", bInput[threadIdx.x], pivot);
        atomicAdd( &(d_gt[blockIdx.x]), 1);
    }
    
  }
  __syncthreads();


  if (threadIdx.x == 0){
      lThisBlock = d_leq[blockIdx.x];
      lOffset = l+atomicAdd(d_leq_val, lThisBlock);
  }
  if (threadIdx.x == 1){
      rThisBlock = d_gt[blockIdx.x];
      rOffset = r-atomicAdd(d_gt_val, rThisBlock);
  }

  __syncthreads();

  if(threadIdx.x == 0){
    int m = 0;
    int n = 0;
    for(int j = 0; j < THREADS_PER_BLOCK; j++){
      int chk = l + blockIdx.x*THREADS_PER_BLOCK + j;
      if(chk <= (r-1) ){
	if(bInput[j] <= pivot){
	  output[lOffset + m] = bInput[j];
	  ++m;
	} else {
	  output[rOffset - n] = bInput[j];
	  ++n;
	}
      }
    }
  }

  __syncthreads();

  if((blockIdx.x == 0) && (threadIdx.x == 0)){
    int pOffset = l;
    for(int k = 0; k < nBlocks; k++)
      pOffset += d_leq[k];

    output[pOffset] = pivot;
    endpts->l = (pOffset - 1);
    endpts->r = (pOffset + 1);
  }

  return;
}

void gqSort(float ls[], int l, int r, int length){
  //if (r - l) >= 1
  if((r - l) >= 1){


    //1. grab pivot
    float pivot = ls[r];

    //2. set-up gpu vars
    int numBlocks = (r - l) / THREADS_PER_BLOCK;
    if((numBlocks * THREADS_PER_BLOCK) < (r - l))
      numBlocks++;

    float * d_ls;
    float * d_ls2;
    vars endpts;
    endpts.l = l;
    endpts.r = r;

    vars * d_endpts;
    int * d_leq, * d_gt, *d_leq_val, *d_gt_val;
    int size = sizeof(float);
    cudaMalloc(&(d_ls), size*length);
    cudaMalloc(&(d_ls2), size*length);
    cudaMalloc(&(d_endpts), sizeof(vars));
    cudaMalloc(&(d_leq), 4*numBlocks);
    cudaMalloc(&(d_gt), 4*numBlocks);
    cudaMalloc(&d_leq_val, 4);
    cudaMalloc(&d_gt_val, 4);
//    sdkStartTimer(&uploadTimer); 

    cudaMemcpy(d_ls, ls, size*length, cudaMemcpyHostToDevice);
    cudaMemcpy(d_ls2, ls, size*length, cudaMemcpyHostToDevice);
//    sdkStopTimer(&downloadTimer); 

    //3. call gpuPartition function
//    sdkStartTimer(&totalTimer); 
    gpuPartitionSwap<<<numBlocks, THREADS_PER_BLOCK>>>(d_ls, d_ls2, d_endpts, pivot, l, r, d_leq, d_gt, d_leq_val, d_gt_val, numBlocks);
//    sdkStopTimer(&totalTimer); 

    //4. Retrieve sorted list and other variables
    cudaMemcpy(ls, d_ls2, size*length, cudaMemcpyDeviceToHost);
    cudaMemcpy(&(endpts), d_endpts, sizeof(vars), cudaMemcpyDeviceToHost);

    cudaThreadSynchronize();
    //5.recursively call on left/right sections of list generated by gpuPartition

    cudaFree(d_ls);
    cudaFree(d_ls2);
    cudaFree(d_endpts);
    cudaFree(d_leq);
    cudaFree(d_gt);

    if(endpts.l >= l)
      gqSort(ls, l, endpts.l, length);
    if(endpts.r <= r)
      gqSort(ls, endpts.r, r, length);
  
  }

  return;
}

/* gpu_quicksort
 *
 * This is a function meant to set up the custom 'data' struct array
 * used by the gpu implementation of quicksort, as well as to calculate
 * the time of execution of the sorting algorithm.
 *
 * Parameters:
 * unsorted: The array of floats to be sorted
 * length: The length of the unsorted and sorted arrays
 * sorted: An output parameter, to be filled with the sorted array.
 *
 * Output:
 * time: This function returns the time of execution required by the
 *       sorting algorithm
 */
double gpu_quicksort(float unsorted[], float sorted[], int length){
  time_t start, end;
  double time;
  double *dev_values;
//  for(int i = 0; i < length; i++)
//    sorted[i] = unsorted[i];

  cudaMalloc((void**) &dev_values, length);
  sdkStartTimer(&uploadTimer);
  cudaMemcpy(dev_values, unsorted, length, cudaMemcpyHostToDevice);
  sdkStopTimer(&uploadTimer); 
  sdkStartTimer(&totalTimer); 
  quicksort <<< length / THREADS_PER_BLOCK, length / THREADS_PER_BLOCK, THREADS_PER_BLOCK >>> (unsorted);
//  gqSort(sorted, 0, length - 1, length);
  end = clock();
  time = ((double) end - start) / CLOCKS_PER_SEC;
  cudaThreadSynchronize(); 
  sdkStopTimer(&totalTimer); 
  sdkStartTimer(&downloadTimer);
  cudaMemcpy((void *)sorted, (void *)dev_values, length*sizeof(float), cudaMemcpyDeviceToHost);
  sdkStopTimer(&downloadTimer); 
  cudaFree(dev_values);
  return time;
}
#define SHAREDSIZE 8000  /* Should be changed to dynamically detect shared
                             memory size if at all possible.  */

//Forward declarations
__global__ void GPUMerge(float *d_list, int len, int stepSize,
                         int eltsPerThread);

/* Mergesort definition.  Takes a pointer to a list of floats.
 * the length of the list, the number of threads per block, and 
 * the number of blocks on which to execute.  
 * Puts the list into sorted order in-place.*/
void MergeSort(float *h_list, int len, int threadsPerBlock, int blocks) {

    float *d_list;
    if ( (cudaMalloc((void **) &d_list, len*sizeof(float))) == 
         cudaErrorMemoryAllocation) {
        printf("Error:  Insufficient device memory at line %d\n", __LINE__);
        return;
    }

  sdkStartTimer(&uploadTimer);
    cudaMemcpy(d_list, h_list, len*sizeof(float), cudaMemcpyHostToDevice);
  sdkStopTimer(&uploadTimer);

    int stepSize = ceil(len/float(threadsPerBlock*blocks));
    int eltsPerThread = ceil(stepSize/threadsPerBlock);
    int maxStep = SHAREDSIZE/sizeof(float);

    if (maxStep < stepSize) {
        stepSize = maxStep;
    }

  sdkStartTimer(&totalTimer);
    GPUMerge<<<blocks, threadsPerBlock>>>(d_list, len, stepSize,
                                          eltsPerThread);
  sdkStopTimer(&totalTimer);

  sdkStartTimer(&downloadTimer);
    cudaMemcpy(h_list, d_list, len*sizeof(float), cudaMemcpyDeviceToHost);
  sdkStopTimer(&downloadTimer);
    cudaFree(d_list);
}

/* Mergesort definition.  Takes a pointer to a list of floats, the length
 * of the list, and the number of list elements given to each thread.
 * Puts the list into sorted order in-place.*/
__global__ void GPUMerge(float *d_list, int len, int stepSize,
                         int eltsPerThread){

    int my_start, my_end; //indices of each thread's start/end

    //Declare counters requierd for recursive mergesort
    int l_start, r_start; //Start index of the two lists being merged
    int old_l_start;
    int l_end, r_end; //End index of the two lists being merged
    int headLoc; //current location of the write head on the newList
    short curList = 0; /* Will be used to determine which of two lists is the
                        * most up-to-date, since merge sort is not an in-place
                        * sorting algorithm. */

    //Attempt to allocate enough shared memory for this block's list...
    //Note that mergesort is not an in-place sort, so we need double memory.
    __shared__ float subList[2][SHAREDSIZE/sizeof(float)];

    //Load memory
    int index = blockIdx.x*blockDim.x + threadIdx.x;
    for (int i = 0; i < eltsPerThread; i++){
        if (index + i < len){
            subList[curList][eltsPerThread*threadIdx.x + i]=d_list[index + i];
        }
    }

    //Wait until all memory has been loaded
    __syncthreads();
    
    //Merge the left and right lists.
    for (int walkLen = 1; walkLen < len; walkLen *= 2) { 
        //Set up start and end indices.
        my_start = eltsPerThread*threadIdx.x;
        my_end = my_start + eltsPerThread;
        l_start = my_start;

        while (l_start < my_end) { 
            old_l_start = l_start; //l_start will be getting incremented soon.
            //If this happens, we are done.
            if (l_start > my_end){
                l_start = len;
                break;
            }
            
            l_end = l_start + walkLen;
            if (l_end > my_end) {
                l_end = len;
            }
            
            r_start = l_end;
            if (r_start > my_end) {
                r_end = len;
            }
            
            r_end = r_start + walkLen;
            if (r_end > my_end) {
                r_end = len;
            }
            
            for (int i = 0; i < walkLen; i++){
                if (subList[curList][l_start] < subList[curList][r_start]) {
                    subList[!curList][headLoc] = subList[curList][l_start];
                    l_start++;
                    headLoc++;
                    //Check if l is now empty
                    if (l_start == l_end) {
                        for (int j = r_start; j < r_end; j++){
                            subList[!curList][headLoc] = 
                                subList[curList][r_start];
                            r_start++;
                            headLoc++;
                    }
                    } 
                }
                else {
                    subList[!curList][headLoc] = subList[curList][r_start];
                    r_start++;
                    //Check if r is now empty
                    if (r_start == r_end) {
                        for (int j = l_start; j < l_end; j++){
                            subList[!curList][headLoc] = 
                                subList[curList][r_start];
                            r_start++;
                            headLoc++;
                        }
                    } 
                }
            }

            l_start = old_l_start + 2*walkLen;
            curList = !curList;
        }
    }
    
    return;

    //subList[blockIdx

    //...otherwise, we use global memory...
    /*
    if ( (subList = cudaMalloc(stepsize*sizeof(float)) != NULL ) {
            //   do some shit.
            
        }    
    */

    //...otherwise, we give up.

}


int
main( int argc, char** argv)
{ 

  // Create timers for each sort
    struct timeval time_start, time_end, program_start, program_end;
    gettimeofday(&program_start, NULL);	

    sdkCreateTimer(&uploadTimer);
    sdkCreateTimer(&downloadTimer);
    sdkCreateTimer(&bucketTimer);
    sdkCreateTimer(&mergeTimer);
    sdkCreateTimer(&totalTimer);
    sdkCreateTimer(&cpuTimer);
	int numElements = 0;
	int method=0;
    // Number of elements in the test bed
    	gettimeofday(&time_start, NULL);
#ifdef READING_FROM_BINARY
        numElements = atoi(argv[2]);
	if (argc == 4)
		method = atoi(argv[3]);
#else
       	if(strcmp(argv[1],"r") ==0) {
	numElements = SIZE; 
	}
	else {
		FILE *fp;
	fp = fopen(argv[1],"r");
	if(fp == NULL) {
	      cout << "Error reading file" << endl;
	      exit(EXIT_FAILURE);
	      }
	int count = 0;
	float c;

	while(fscanf(fp,"%f",&c) != EOF) {
	 count++;
}
	fclose(fp);
	numElements = count;
}
#endif

	cout << "Sorting list of " << numElements << " floats\n";
	// Generate random data
	// Memory space the list of random floats will take up
	int mem_size = numElements * sizeof(float); 
	// Allocate enough for the input list
	float *cpu_idata = (float *)malloc(mem_size);
	// Allocate enough for the output list on the cpu side
	float *cpu_odata = (float *)malloc(mem_size);
	// Allocate enough memory for the output list on the gpu side
	float *gpu_odata = (float *)malloc(mem_size);

	float datamin = FLT_MAX; 
	float datamax = -FLT_MAX; 
	if(strcmp(argv[1],"r")==0) {

	for (int i = 0; i < numElements; i++) {
	// Generate random floats between 0 and 1 for the input data
		cpu_idata[i] = ((float) rand() / RAND_MAX); 
	//Compare data at index to data minimum, if less than current minimum, set that element as new minimum
		datamin = min(cpu_idata[i], datamin);
	//Same as above but for maximum
		datamax = max(cpu_idata[i], datamax);
	}
}	else {
	FILE *fp;
#ifdef READING_FROM_BINARY
	fp = fopen(argv[1],"rb");
	fread(cpu_idata,sizeof(float),numElements,fp);
	fclose(fp);
   	gettimeofday(&time_end, NULL);
	printf("FileInput %lf seconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);

    	gettimeofday(&time_start, NULL);	
	
/*	for(int i = 0; i < numElements; i++) {
	datamin = min(cpu_idata[i],datamin);
	datamax = max(cpu_idata[i],datamax);
	}*/

	datamin = 0.0;
	datamax = 16777215.0;
#else
	fp = fopen(argv[1],"r");
	for(int i = 0; i < numElements; i++) {
	fscanf(fp,"%f",&cpu_idata[i]);
	datamin = min(cpu_idata[i],datamin);
	datamax = max(cpu_idata[i],datamax);
	}
	fclose(fp);
   	gettimeofday(&time_end, NULL);
	printf("FileInput %lf seconds\n",((time_end.tv_sec * 1000000 + time_end.tv_usec) - (time_start.tv_sec * 1000000 + time_start.tv_usec))/1000000.0);
    	gettimeofday(&time_start, NULL);	
#endif
	}
#ifndef CPU
	cout << "Sorting on GPU..." << flush; 
	// GPU Sort
	if(method == 0)
	for (int i = 0; i < TEST; i++) 
                MergeSort(cpu_idata, numElements, THREADS_PER_BLOCK, numElements/THREADS_PER_BLOCK);
//		gpu_quicksort(cpu_idata, gpu_odata, numElements);		
	else {
	// Using bitonic sort
	for (int i = 0; i < TEST; i++) 
		bitonic_sort(cpu_idata, gpu_odata, numElements);		
	}
#endif

#ifdef VERIFY
	cout << "Sorting on CPU..." << flush; 
	// CPU Sort
	sdkStartTimer(&cpuTimer); 

	memcpy(cpu_odata, cpu_idata, mem_size);
        qsort(cpu_odata, numElements, sizeof(float), compare);
	sdkStopTimer(&cpuTimer); 
	cout << "done.\n";
	cout << "Checking result..." << flush; 
	// Result checking
	int count = 0; 
	for(int i = 0; i < numElements; i++)
		if(fabs(cpu_odata[i] - gpu_odata[i])> 0.0002)
		{
			printf("Sort missmatch on element %d: \n", i); 
			printf("CPU = %f : GPU = %f\n", cpu_odata[i], gpu_odata[i]); 
			count++; 
			break; 
		}
	if(count == 0) cout << "PASSED.\n";
	else cout << "FAILED.\n";
#endif
	// Timer report
	printf("GPU iterations: %d\n", TEST); 
#ifdef TIMER
#ifdef VERIFY
	printf("Total CPU execution time: %lf seconds\n", sdkGetTimerValue(&cpuTimer)/1000);
#endif
#ifndef CPU
	printf("Total GPU Sort execution time: %lf seconds\n", sdkGetTimerValue(&totalTimer) /1000);
	printf("    - Upload		: %lf seconds\n", sdkGetTimerValue(&uploadTimer) /1000);
	printf("    - Download		: %lf seconds\n", sdkGetTimerValue(&downloadTimer) /1000);
	printf("    - Bucket sort	: %lf seconds\n", sdkGetTimerValue(&bucketTimer) /1000);
	printf("    - Merge sort	: %lf seconds\n", sdkGetTimerValue(&mergeTimer) /1000);
#endif
#endif

	
    // Release memory
    gettimeofday(&program_end, NULL);
    printf("HGProfile: Total %lf\n",((program_end.tv_sec * 1000000 + program_end.tv_usec) - (program_start.tv_sec * 1000000 + program_start.tv_usec))-sdkGetTimerValue(&cpuTimer)/1000);
    sdkDeleteTimer(&uploadTimer);
    sdkDeleteTimer(&downloadTimer);
    sdkDeleteTimer(&bucketTimer);
    sdkDeleteTimer(&mergeTimer);
    sdkDeleteTimer(&totalTimer);
    sdkDeleteTimer(&cpuTimer);
	free(cpu_idata); 
}

