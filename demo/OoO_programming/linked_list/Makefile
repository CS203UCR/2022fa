all: linked_list


#arraySort

EXTRA_CFLAGS = -DHAVE_LINUX_PERF_EVENT_H
CFLAGS = -O3 -g 

linked_list: linked_list.c perfstats.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) perfstats.c  linked_list.c -o linked_list
	
clean:
		rm -f linked_list 
