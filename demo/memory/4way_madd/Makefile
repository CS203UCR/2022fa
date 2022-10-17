all: madd_intel madd_nano madd_A_fission
 
EXTRA_CFLAGS = -DHAVE_LINUX_PERF_EVENT_H -g

madd_nano: madd.c 
	$(CC) -O3 $(EXTRA_CFLAGS) -DNANO madd.c -o madd_nano
madd_intel: madd.c 
	$(CC) -O3 $(EXTRA_CFLAGS) madd.c -o madd_intel


madd_A_fission: madd_fission.c 
	$(CC) $(EXTRA_CFLAGS)  -DA madd_fission.c -o madd_A_fission

clean:
		rm -f madd_intel madd_nano *_O3 *~ madd_A_fission cachegrind* *.perf
		