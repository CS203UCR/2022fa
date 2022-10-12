all: mm blockmm blockmm_transpose

mm: mm.c
	gcc -O3 mm.c -o mm 

blockmm: blockmm.c 
	gcc -O3 blockmm.c -o blockmm 

blockmm_transpose: blockmm_transpose.c 
	gcc -O3 blockmm_transpose.c -o blockmm_transpose

blockmm_transpose_prefetch: blockmm_transpose.c 
	gcc -O3 -fprefetch-loop-arrays blockmm_transpose.c -o blockmm_transpose

clean:
		rm -f blockmm mm blockmm_transpose
