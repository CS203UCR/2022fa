all: madd arraySort bitonic quicksort


madd: madd.c
	gcc madd.c -o madd 

arraySort: arraySort.cpp
	g++ -O0 arraySort.cpp -o arraySort


bitonic: bitonic.cu
	/usr/local/cuda/bin/nvcc -O3 bitonic.cu -o bitonic
        
quicksort: quicksort.cu 
	/usr/local/cuda/bin/nvcc -O3 quicksort.cu -o quicksort
        
clean:
		rm -f madd arraySort bitonic quicksort
