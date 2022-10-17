all: matvec

EXTRA_CFLAGS = -DHAVE_LINUX_PERF_EVENT_H -g -O3

matvec: matvec.c 
	$(CC) $(EXTRA_CFLAGS)  matvec.c -o matvec

clean:
		rm -f matvec
		