all:  arithmetics_A arithmetics_B swap flip_array


#arraySort

EXTRA_CFLAGS = -DHAVE_LINUX_PERF_EVENT_H
CFLAGS = -O3

arithmetics_A: arithmetics.c 
	$(CC) -DA -O0 $(EXTRA_CFLAGS) arithmetics.c -o arithmetics_A
	$(CC) -DA -O0 $(EXTRA_CFLAGS) -S arithmetics.c -o A.s

arithmetics_B: arithmetics.c 
	$(CC) -DB -O0 $(EXTRA_CFLAGS) arithmetics.c -o arithmetics_B
	$(CC) -DB -O0 $(EXTRA_CFLAGS) -S arithmetics.c -o B.s

swap: swap.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) swap.c -o swap

flip_array: flip_array.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) flip_array.c -o flip_array
	$(CC) -S -O2 $(EXTRA_CFLAGS) flip_array.c -o flip_array.s
	
clean:
		rm -f arithmetics arithmetics_A arithmetics_B flip_array swap
