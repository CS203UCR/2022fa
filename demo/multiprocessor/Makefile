all: testloop testloop_O3 coherence blockmm blockmm_pthread value_of_i vadd_sse fence mfence vadd_sse_two_threads vadd_sse_two_threads_chunk vadd_mt vadd_mt_chunk

testloop_O3: testloop.c
	gcc -O3 -g testloop.c -lpthread -o testloop_O3
#	otool -tV testloop_O3 > testloop_O3.dump 


testloop: testloop.c
	gcc -O0 -g testloop.c -lpthread -o testloop 

testloop_O3_volatile: testloop.c
	gcc -O3 -Dvolatile testloop.c -lpthread -o testloop_O3
#	otool -tV testloop_O3 > testloop_O3.dump 


testloop_volatile: testloop.c
	gcc -O0 -g testloop.c -lpthread -o testloop 
#	otool -tV testloop > testloop.dump 

coherence: coherence.c
	gcc -O0 coherence.c -lpthread -o coherence 

value_of_i: value_of_i.c
	gcc value_of_i.c -lpthread -o value_of_i

blockmm_pthread: blockmm_pthread.c
	gcc -O3 blockmm_pthread.c -lpthread -o blockmm_pthread

blockmm: blockmm.c
	gcc -O3 blockmm.c -lpthread -o blockmm 

vadd_sse: vadd.c 
	$(CC) -O3 $(EXTRA_CFLAGS) -DSSE vadd.c -o vadd_sse

vadd_sse_pthread: vadd_pthread.c 
	$(CC) -O3 $(EXTRA_CFLAGS) -pthread -DSSE vadd_pthread.c -o vadd_sse_pthread 

vadd_sse_two_threads: vadd_two_threads.c 
	$(CC) -O3 $(EXTRA_CFLAGS) -pthread -DSSE vadd_two_threads.c -o vadd_sse_two_threads

vadd_sse_two_threads_chunk: vadd_two_threads.c 
	$(CC) -O3 $(EXTRA_CFLAGS) -lpthread -DSSE -DCHUNK vadd_two_threads.c -o vadd_sse_two_threads_chunk

vadd_mt: vadd_mt.c perfstats.c
	$(CC) -O3 $(EXTRA_CFLAGS) -DHAVE_LINUX_PERF_EVENT_H -lpthread -DSSE vadd_mt.c perfstats.c -o vadd_mt

vadd_mt_chunk: vadd_mt.c 
	$(CC) -O3 $(EXTRA_CFLAGS) -DHAVE_LINUX_PERF_EVENT_H -lpthread -DSSE -DCHUNK vadd_mt.c perfstats.c -o vadd_mt_chunk 

pthread_overhead: pthread_overhead.c
	$(CC) -O3 $(EXTRA_CFLAGS) -lpthread -DSSE pthread_overhead.c -o pthread_overhead

fence: fence.c 
	$(CC) -O3 $(EXTRA_CFLAGS) fence.c -o fence -lpthread

mfence: fence.c 
	$(CC) -O3 -DMFENCE $(EXTRA_CFLAGS) fence.c -o mfence -lpthread


clean:
		rm -f testloop testloop_O3 coherence blockmm blockmm_pthread value_of_i vadd_sse fence mfence vadd_sse_two_threads vadd_sse_two_threads_chunk vadd_mt vadd_mt_chunk *.dump *~ value_of_i vadd_sse vadd_sse_pthread
		rm -rf *.dSYM
