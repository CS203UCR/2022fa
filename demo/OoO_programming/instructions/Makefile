all: arithmetics arithmetics_3_A arithmetics_3_B arithmetics_2_A arithmetics_2_B 


#arraySort

EXTRA_CFLAGS = -DHAVE_LINUX_PERF_EVENT_H
#CFLAGS = -O3

arithmetics: arithmetics.c 
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) arithmetics.c -o arithmetics

arithmetics_2_A: arithmetics_2.c 
	$(CC) -DA $(CFLAGS) $(EXTRA_CFLAGS) arithmetics_2.c -o arithmetics_2_A
	$(CC) -DA $(CFLAGS) $(EXTRA_CFLAGS) -S arithmetics_2.c -o A.s

arithmetics_2_B: arithmetics_2.c 
	$(CC) -DB $(CFLAGS) $(EXTRA_CFLAGS) arithmetics_2.c -o arithmetics_2_B
	$(CC) -DB $(CFLAGS) $(EXTRA_CFLAGS) -S arithmetics_2.c -o B.s

arithmetics_3_A: arithmetics_3.c 
	$(CC) -DA $(CFLAGS) $(EXTRA_CFLAGS) arithmetics_3.c -o arithmetics_3_A
	$(CC) -DA $(CFLAGS) $(EXTRA_CFLAGS) -S arithmetics_3.c -o A.s

arithmetics_3_B: arithmetics_3.c 
	$(CC) -DB $(CFLAGS) $(EXTRA_CFLAGS) arithmetics_3.c -o arithmetics_3_B
	$(CC) -DB $(CFLAGS) $(EXTRA_CFLAGS) -S arithmetics_3.c -o B.s

	
clean:
		rm -f arithmetics arithmetics_3_A arithmetics_3_B arithmetics_2_A arithmetics_2_B 
