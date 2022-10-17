all: madd_A madd_B

EXTRA_CFLAGS = -DHAVE_LINUX_PERF_EVENT_H -g -O3

madd_A: madd_A.c 
	$(CC) $(EXTRA_CFLAGS)  madd_A.c -o madd_A

madd_B: madd_B.c 
	$(CC) $(EXTRA_CFLAGS)  madd_B.c -o madd_B

perfstats.o: perfstats.c 
	$(CC) -O3 $(EXTRA_CFLAGS) perfstats.c -c

clean:
		rm -f madd_A madd_B 
		