all: transpose transpose_prefetch

transpose: transpose.cpp
	g++ -msse4.1 -mavx -O3 transpose.cpp -o transpose 

transpose_prefetch: transpose.cpp
	g++ -msse4.1 -mavx -O3 -DENABLE_PREFETCH transpose.cpp -o transpose_prefetch 

clean:
		rm -f blockmm_sse blockmm blockmm_sse_prefetch transpose transpose_prefetch
