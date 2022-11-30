#include"function_map.hpp"
#include<cstdint>
#include<thread>
#include<mutex>
#include"threads.hpp"
#include"pthread.h"

//START_UNTHREADED
volatile uint64_t histogram[256];
extern "C"
void __attribute__((noinline)) unthreaded_histogram(uint64_t* data, uint64_t size)
{
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			histogram[b]++;
		}
	}
}

extern "C"
uint64_t* run_unthreaded_histogram(uint64_t thread_count, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}
	unthreaded_histogram(data, size);
	return data;
}
//END_UNTHREADED
FUNCTION(one_array_2arg, run_unthreaded_histogram);


//START_THREADED
std::mutex lock;

extern "C"
void __attribute__((noinline)) threaded_histogram(uint64_t * data, uint64_t size) {

	
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			lock.lock(); // we have to take a lock when we access shared data.
			histogram[b]++;
			lock.unlock();
		}
	}
}

#define force_cast(X) reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(X))
extern "C"
uint64_t* run_threaded_histogram(uint64_t thread_count, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}
	std::thread **threads = new std::thread*[thread_count];
	int chunk_length = size/thread_count; // chunk_length might be rounded down
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i] = new std::thread(threaded_histogram, &data[chunk_length * i], chunk_length);
	}

	threaded_histogram(&data[chunk_length * (thread_count - 1)],
			   size-(chunk_length * (thread_count - 1)));  // catch last bit due to rounding.
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i]->join();
	}
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		delete threads[i];
	}
	delete threads;

	return data;
}
//END_THREADED

FUNCTION(one_array_2arg, run_threaded_histogram);

//START_FINE
std::mutex *fine_locks[256];

extern "C"
void __attribute__((noinline)) fine_locks_histogram(uint64_t * data, uint64_t size) {

	
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			fine_locks[b]->lock(); // we have to take a lock when we access shared data.
			histogram[b]++;
			fine_locks[b]->unlock(); 
		}
	}
}

extern "C"
uint64_t* run_fine_locks_histogram(uint64_t thread_count, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
		fine_locks[i] = new std::mutex();
	}
	std::thread **threads = new std::thread*[thread_count];
	int chunk_length = size/thread_count; // chunk_length might be rounded down
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i] = new std::thread(fine_locks_histogram, &data[chunk_length * i], chunk_length);
	}

	fine_locks_histogram(&data[chunk_length * (thread_count - 1)],
			   size-(chunk_length * (thread_count - 1)));  // catch last bit due to rounding.
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i]->join();
	}
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		delete threads[i];
	}
	delete threads;

	for(int i =0; i < 256;i++) {
		delete fine_locks[i];
	}

	
	return data;
}
//END_FINE
FUNCTION(one_array_2arg, run_fine_locks_histogram);

//START_PRIVATE
extern "C"
void __attribute__((noinline)) private_histogram(int id, int thread_count, uint64_t *buckets, uint64_t * data, uint64_t size) {

	
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			buckets[b*thread_count + id]++;
		}
	}	

}

extern "C"
uint64_t* run_private_histogram(uint64_t thread_count, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {

	uint64_t * private_buckets = new uint64_t[thread_count * 256]();
	
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}
	
	std::thread **threads = new std::thread*[thread_count];
	int chunk_length = size/thread_count; // chunk_length might be rounded down
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i] = new std::thread(private_histogram,
					     i,
					     thread_count,
					     private_buckets,
					     &data[chunk_length * i],
					     chunk_length);
	}

	private_histogram(thread_count - 1,
			  thread_count,
			  private_buckets,
			  &data[chunk_length * (thread_count - 1)],
			  size-(chunk_length * (thread_count - 1)));  // catch last bit due to rounding.
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i]->join();
	}

	for(int b = 0; b < 256; b++) {
		for(unsigned int id = 0; id < thread_count; id++) {
			histogram[b] += private_buckets[b*thread_count + id]++;
		}
	}

	for(unsigned int i = 0; i < thread_count - 1; i++) {
		delete threads[i];
	}
	delete threads;
	delete private_buckets;

	return data;
}
//END_PRIVATE
FUNCTION(one_array_2arg, run_private_histogram);

//START_PRIVATE2
extern "C"
void __attribute__((noinline)) private2_histogram(int id, int thread_count, uint64_t *buckets, uint64_t * data, uint64_t size) {
	
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			buckets[id*256 + b]++;
		}
	}
}

extern "C"
uint64_t* run_private2_histogram(uint64_t thread_count, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {

	uint64_t * private_buckets = new uint64_t[thread_count * 256]();

	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}
	
	std::thread **threads = new std::thread*[thread_count];
	int chunk_length = size/thread_count; // chunk_length might be rounded down
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i] = new std::thread(private2_histogram,
					     i,
					     thread_count,
					     private_buckets,
					     &data[chunk_length * i],
					     chunk_length);
	}

	private2_histogram(thread_count - 1,
			  thread_count,
			  private_buckets,
			  &data[chunk_length * (thread_count - 1)],
			  size-(chunk_length * (thread_count - 1)));  // catch last bit due to rounding.
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i]->join();
	}


	for(unsigned int id = 0; id < thread_count; id++) {
		for(int b = 0; b < 256; b++) {
			histogram[b] += private_buckets[id*256 + b]++;
		}
	}

	for(unsigned int i = 0; i < thread_count - 1; i++) {
		delete threads[i];
	}
	delete threads;
	delete private_buckets;

	return data;
}
//END_PRIVATE2
FUNCTION(one_array_2arg, run_private2_histogram);


//START_OPENMP

extern "C"
uint64_t* run_openmp_histogram(uint64_t thread_count, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {

	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}

#pragma omp parallel for 
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
#pragma omp critical 
			histogram[b]++;
		}
	}
	return data;
}
//END_OPENMP
FUNCTION(one_array_2arg, run_openmp_histogram);

//START_OPENMP_PRIVATE

extern "C"
uint64_t* run_openmp_private_histogram(uint64_t thread_count, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {

	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}

#pragma omp parallel for
	for(uint64_t ii = 0; ii < size; ii+=arg1) {
		uint64_t my_histogram[256];
		for(int i =0; i < 256;i++) {
			my_histogram[i] = 0;
		}
		for(uint64_t i = ii; i < size && i < ii + arg1; i++) {
			
			for(int k = 0; k < 64; k+=8) {
				uint8_t b = (data[i] >> k)& 0xff;
				my_histogram[b]++;
			}
		}
#pragma omp critical 
		for(int i =0; i < 256;i++) {
			histogram[i] += my_histogram[i];
		}
	}
	return data;
}
//END_OPENMP_PRIVATE
FUNCTION(one_array_2arg, run_openmp_private_histogram);


