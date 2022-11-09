all: arraySort 

perfstats.o: perfstats.c perfstats.h
	gcc -DHAVE_LINUX_PERF_EVENT_H -O3 -I$(PWD) $(LIBS) -o perfstats.o -c perfstats.c


arraySort: arraySort.cpp calculate_sum.c perfstats.o
	gcc calculate_sum.c -c 
	g++ -O0 -DHAVE_LINUX_PERF_EVENT_H arraySort.cpp perfstats.o calculate_sum.o -o arraySort

clean:
		rm -f madd arraySort bitonic quicksort arraySortO3
