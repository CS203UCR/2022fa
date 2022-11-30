#include<pthread.h>

[[maybe_unused]]
static void bind_to_core(std::thread & thread, int core) {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);
	int rc = pthread_setaffinity_np(thread.native_handle(),
					sizeof(cpu_set_t), &cpuset);
	if (rc != 0) {
		std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
		exit(1);
	}
}

[[maybe_unused]]
static void bind_to_core(const pthread_t & thread, int core) {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);
	int rc = pthread_setaffinity_np(thread,
					sizeof(cpu_set_t), &cpuset);
	if (rc != 0) {
		std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
		exit(1);
	}
}
