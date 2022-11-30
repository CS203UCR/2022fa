#ifndef ARCHLAB_H_INCLUDED
#define ARCHLAB_H_INCLUDED
#include<stdint.h>

#define GETTIMEOFDAY

#ifdef GETTIMEOFDAY
#include <sys/time.h> // For struct timeval, gettimeofday
#else
#include <time.h> // For struct timespec, clock_gettime, CLOCK_MONOTONIC
#endif

#ifdef __cplusplus
#include<iostream>
#endif

#include <stdlib.h>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)


#define ARCHLAB_COLLECTOR_PCM 0
#define ARCHLAB_COLLECTOR_PAPI 1
#define ARCHLAB_COLLECTOR_PIN 2
#define ARCHLAB_COLLECTOR_NONE 3
#define ARCHLAB_COLLECTOR_ALLCORE 4

#ifdef __cplusplus
extern "C" {
#endif
  
	void start_timing(const char * name,...);
	void stop_timing();
	void set_cpu_clock_frequency(int mhz);  

	static inline double wall_time ()
	{

#ifdef GETTIMEOFDAY
		struct timeval t;
		gettimeofday (&t, NULL);
		return 1.0*t.tv_sec + 1.e-6*t.tv_usec;
#else
		struct timespec t;
		clock_gettime (CLOCK_MONOTONIC, &t);
		return 1.0*t.tv_sec + 1.e-9*t.tv_nsec;
#endif
	}

	uint64_t si_parse(const char *);
  
  
	// default system random number generator
	static inline uint64_t rand_int() {
		return rand()*RAND_MAX + rand();
	}

	static inline double rand_double() {
		return (rand() + 0.0)/(RAND_MAX + 0.0);
	}


	//https://en.wikipedia.org/wiki/Xorshift
	inline static uint64_t fast_rand(uint64_t * seed)
	{
		if (*seed == 0) {
			*seed = 1;
		}
		uint64_t x = *seed;
		x ^= x << 13;
		x ^= x >> 7;
		x ^= x << 17;
		*seed = x;
		return *seed;
	}

#define TAP(a) (((a) == 0) ? 0 : ((1ull) << (((uint64_t)(a)) - (1ull))))

#define RAND_LFSR_DECL(BITS, T1, T2, T3, T4)				\
	inline static uint##BITS##_t RandLFSR##BITS(uint##BITS##_t *seed) { \
		if (*seed == 0) {					\
			*seed = 1;					\
		}							\
 									\
		const uint##BITS##_t mask = TAP(T1) | TAP(T2) | TAP(T3) | TAP(T4); \
		*seed = (*seed >> 1) ^ (uint##BITS##_t)(-(*seed & (uint##BITS##_t)(1)) & mask); \
		return *seed;						\
	}

	RAND_LFSR_DECL(64, 64,63,61,60);
	RAND_LFSR_DECL(32, 32,30,26,25);
	RAND_LFSR_DECL(16, 16,14,13,11);
	RAND_LFSR_DECL(8 ,  8, 6, 5, 4);

	// Very fast (but not so random) random number generator.
	inline static uint64_t fast_rand2(uint64_t * x) {
		return RandLFSR64(x);
	}

  
	extern int * cpu_frequencies_array;

	// These are fo signaling to pin tools to start and stop tracking
	// stats at a very fine grain.
	void archlab_start_quick() __attribute__((noinline));
	void archlab_start_quick();
	void archlab_stop_quick() __attribute__((noinline));
	void archlab_stop_quick();

	int archlab_canary(long int count);

#ifdef __cplusplus
} // C linkage
#endif

#endif
