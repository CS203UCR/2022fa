
#define NUM_OF_THREADS 0
#define CPU_TIME 1
#define HOST2GPU 2
#define GPU_KERNEL 3
#define GPU2HOST 4

double perf_stats[5];

void vector_mul_double_cuda(double *va, double *vb, unsigned long int size, int p, int iter);
void vector_mul_double(double *va, double *vb, unsigned long int size, int p, int iter);
