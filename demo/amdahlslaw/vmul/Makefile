all: vmul 


vmul: vmul.o vmul_cpu.o vmul_cuda.o
	gcc -O3 *.o -lcuda -lcudart -L/usr/local/cuda/lib64 -o vmul
#vmul: vmul.c vmul_cpu.c vmul_cuda.cu
#	/usr/local/cuda/bin/nvcc -O3 vmul.c vmul_cpu.c vmul_cuda.cu -lcuda -lcudart -o vmul

vmul.o: vmul.c
	gcc -O3 vmul.c -c

vmul_cuda.o: vmul_cuda.cu
	/usr/local/cuda/bin/nvcc -O3 -w vmul_cuda.cu -c
        
vmul_cpu.o: vmul_cpu.c 
	gcc -O3 vmul_cpu.c -c
        
clean:
		rm -f vmul *.o
