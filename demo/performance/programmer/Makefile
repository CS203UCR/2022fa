all: madd_A madd_B madd_A_O3 madd_B_O3 madd_change_freq
 
EXTRA_CFLAGS = -DHAVE_LINUX_PERF_EVENT_H -g

madd: madd.c 
	$(CC) -O0 $(EXTRA_CFLAGS) madd.c -o madd

madd_A: madd_A.c 
	$(CC) -O0 $(EXTRA_CFLAGS)  madd_A.c -o madd_A

madd_B: madd_B.c 
	$(CC) -O0 $(EXTRA_CFLAGS)  madd_B.c -o madd_B
madd_A_O3: madd_A.c 
	$(CC) $(EXTRA_CFLAGS) -O3  madd_A.c -o madd_A_O3

madd_B_O3: madd_B.c 
	$(CC) $(EXTRA_CFLAGS) -O3  madd_B.c -o madd_B_O3

perfstats.o: perfstats.c 
	$(CC) -O3 $(EXTRA_CFLAGS) perfstats.c -c

madd_change_freq: madd_change_freq.c perfstats.o
	$(CC) -O3 $(EXTRA_CFLAGS) -DA madd_change_freq.c perfstats.o -o madd_change_freq

clean:
		rm -f madd_A madd_B madd_change_freq *_O3 *~
		#hotspot hotspot_omp
		