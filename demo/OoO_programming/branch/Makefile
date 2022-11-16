all: branch_A branch_B branch_C branch_C branch_D


EXTRA_CFLAGS = -DHAVE_LINUX_PERF_EVENT_H -g
#-lprofiler

branch_A: branch.c 
	$(CC) -O3 $(CFLAGS) $(EXTRA_CFLAGS) -DBITWISE branch.c -o branch_A

branch_B: branch.c 
	$(CC) -O3 $(CFLAGS) $(EXTRA_CFLAGS) branch.c -o branch_B

branch_C: branch.c 
	$(CC) -O3 $(CFLAGS) $(EXTRA_CFLAGS) -DNO_BRANCH branch.c -o branch_C

branch_D: branch.c 
	$(CC) -O3 $(CFLAGS) $(EXTRA_CFLAGS) -DCOND branch.c -o branch_D

clean:
		rm -f branch_A branch_B branch_A branch_B branch_C branch_D
