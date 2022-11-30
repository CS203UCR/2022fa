#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include <cstdint>
#include <cassert>
#include <dlfcn.h>
#include <vector>
#include <sstream>
#include "archlab.h"
#include "perfstats.h"
#include "function_map.hpp"
#include <omp.h>

uint array_size;

class alloc_test: public benchmark_env<uint64_t*(*)(uint64_t, uint64_t)> {
public:
	void reset_environment(const parameter_map_t & parameters) {}

	void sort_data(parameter_map_t & parameters) {}
	
	std::function<void()> get_function(void * the_func, parameter_map_t & parameters) {
		uint64_t count = boost::any_cast<uint64_t>(parameters["size"]);
		uint64_t seed= boost::any_cast<uint64_t>(parameters["arg1"]);
		auto f = cast_function(the_func);
		return [f, seed, count]() {
			f(count, seed);
		};
	}
};

class one_array: public benchmark_env<uint64_t*(*)(uint64_t *, unsigned long int)> {
	uint64_t * array;
	uint64_t max_size;
public:
	one_array(uint64_t max_size): max_size(max_size) {
		array = new uint64_t[max_size];
	}
	
	void reset_environment(const parameter_map_t & parameters) {
		uint64_t a = 1;
		for (unsigned int i = 0; i < max_size; i++) {
			array[i] = fast_rand(&a);
		}
	}

	void sort_data(parameter_map_t & parameters) {
	        uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
	        std::sort(array, array+size);	
	}

	std::function<void()> get_function(void * the_func, parameter_map_t & parameters) {
		uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
		auto f = cast_function(the_func);
		uint64_t * _array = array;
		return [f, _array, size]() {
			       f(_array, size);
		       };
	}
	~one_array() {
		delete [] array;
	}
};

class two_arrays: public benchmark_env<uint64_t*(*)(uint64_t *, unsigned long int,uint64_t *, unsigned long int)> {
	uint64_t * array1;
	uint64_t * array2;
	uint64_t max_size;
public:
	two_arrays(uint64_t max_size): max_size(max_size) {
		array1 = new uint64_t[max_size];
		array2 = new uint64_t[max_size];
	}
	
	void reset_environment(const parameter_map_t & parameters) {
		uint64_t a = 1;
		for (unsigned int i = 0; i < max_size; i++) {
			array1[i] = fast_rand(&a);
			array2[i] = fast_rand(&a);
		}
	}

	void sort_data(parameter_map_t & parameters) {
	        uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
	        std::sort(array1, array1+size);	

	}
	
	std::function<void()> get_function(void * the_func, parameter_map_t & parameters) {
		uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
		uint64_t size2 = boost::any_cast<uint64_t>(parameters["size2"]);
		auto f = cast_function(the_func);
		uint64_t * _array1 = array1;
		uint64_t * _array2 = array2;
		return [f, _array1,_array2, size, size2]() {
			f(_array1, size, _array2, size2);
		};
	}
	~two_arrays() {
		delete [] array1;
		delete [] array2;
	}
};

class three_arrays: public benchmark_env<uint64_t*(*)(uint64_t *, unsigned long int,
						      uint64_t *, unsigned long int,
						      uint64_t *, unsigned long int)> {
	uint64_t * array1;
	uint64_t * array2;
	uint64_t * array3;
	uint64_t max_size;
public:
	three_arrays(uint64_t max_size): max_size(max_size) {
		array1 = new uint64_t[max_size];
		array2 = new uint64_t[max_size];
		array3 = new uint64_t[max_size];
	}

	void sort_data(parameter_map_t & parameters) {}
	
	void reset_environment(const parameter_map_t & parameters) {
		uint64_t a = 1;
		for (unsigned int i = 0; i < max_size; i++) {
			array1[i] = fast_rand(&a);
			array2[i] = fast_rand(&a);
			array3[i] = fast_rand(&a);
		}
	}
	
	std::function<void()> get_function(void * the_func, parameter_map_t & parameters) {
		uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
		uint64_t size2 = boost::any_cast<uint64_t>(parameters["size2"]);
		uint64_t size3 = boost::any_cast<uint64_t>(parameters["size3"]);
		auto f = cast_function(the_func);
		uint64_t * _array1 = array1;
		uint64_t * _array2 = array2;
		uint64_t * _array3 = array3;
		return [f, _array1,_array2, _array3, size, size2, size3]() {
			f(_array1, size, _array2, size2, _array3, size3);
		};
	}
	
	~three_arrays() {
		delete [] array1;
		delete [] array2;
		delete [] array3;
	}
};

class convolution: public benchmark_env<uint64_t*(*)(uint64_t *, unsigned long int,
						     uint64_t *, unsigned long int,
						     uint64_t *, unsigned long int,
						     int32_t)> {
	uint64_t * array1;
	uint64_t * array2;
	uint64_t * array3;
	uint64_t max_size;
public:
	convolution(uint64_t max_size): max_size(max_size) {
		array1 = new uint64_t[max_size];
		array2 = new uint64_t[max_size];
		array3 = new uint64_t[max_size];
	}
	

	void sort_data(parameter_map_t & parameters) {}

	void reset_environment(const parameter_map_t & parameters) {
		uint64_t a = 1;
		for (unsigned int i = 0; i < max_size; i++) {
			array1[i] = fast_rand(&a);
			array2[i] = fast_rand(&a);
			array3[i] = fast_rand(&a);
		}
	}
	
	std::function<void()> get_function(void * the_func, parameter_map_t & parameters) {
		uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
		uint64_t size2 = boost::any_cast<uint64_t>(parameters["size2"]);
		uint64_t size3 = boost::any_cast<uint64_t>(parameters["size3"]);
		uint64_t tile_size = boost::any_cast<int32_t>(parameters["tile_size"]);
		auto f = cast_function(the_func);
		uint64_t * _array1 = array1;
		uint64_t * _array2 = array2;
		uint64_t * _array3 = array3;
		return [f, _array1,_array2, _array3, size, size2, size3, tile_size]() {
			       f(_array1, size, _array2, size2, _array3, size3, tile_size);
		};
	}
	
	~convolution() {
		delete [] array1;
		delete [] array2;
		delete [] array3;
	}
};

class one_array_1arg: public benchmark_env<uint64_t*(*)(uint64_t *, unsigned long int, unsigned long int)> {
	uint64_t * array;
	uint64_t max_size;
public:
	one_array_1arg(uint64_t max_size): max_size(max_size) {
		array = new uint64_t[max_size];
	}
	
	void reset_environment(const parameter_map_t & parameters) {
		 uint64_t a = 1;
		 for (unsigned int i = 0; i < max_size; i++) {
		 	array[i] = fast_rand(&a);
		 }
	}
	void sort_data(parameter_map_t & parameters) {
	        uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
	        std::sort(array, array+size);	
	}
	
	std::function<void()> get_function(void * the_func, parameter_map_t & parameters) {
		assert(parameters.find("size") != parameters.end());
		assert(parameters.find("arg1") != parameters.end());
		uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
		uint64_t arg1 = boost::any_cast<uint64_t>(parameters["arg1"]);
		auto f = cast_function(the_func);
		uint64_t * _array = array;
		return [f, _array, size, arg1]() {
			       f(_array, size, arg1);
		       };
	}
	~one_array_1arg() {
		delete [] array;
	}
};


class one_array_2arg: public benchmark_env<uint64_t*(*)(unsigned long int, uint64_t *, unsigned long int, unsigned long int, unsigned long int, unsigned long int)> {
	uint64_t * array;
	uint64_t max_size;
public:
	one_array_2arg(uint64_t max_size): max_size(max_size) {
		array = new uint64_t[max_size];
	}
	
	void reset_environment(const parameter_map_t & parameters) {
		uint64_t a = 1;
		for (unsigned int i = 0; i < max_size; i++) {
			array[i] = fast_rand(&a);
		}
	}
	void sort_data(parameter_map_t & parameters) {
	        uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
	        std::sort(array, array+size);	
	}
	
	std::function<void()> get_function(void * the_func, parameter_map_t & parameters) {
		assert(parameters.find("size") != parameters.end());
		assert(parameters.find("arg1") != parameters.end());
		assert(parameters.find("arg2") != parameters.end());
		assert(parameters.find("arg3") != parameters.end());
		assert(parameters.find("thread") != parameters.end());
		uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
		uint64_t arg1 = boost::any_cast<uint64_t>(parameters["arg1"]);
		uint64_t arg2 = boost::any_cast<uint64_t>(parameters["arg2"]);
		uint64_t arg3 = boost::any_cast<uint64_t>(parameters["arg3"]);
		uint64_t tc = boost::any_cast<uint64_t>(parameters["thread"]);
		auto f = cast_function(the_func);
		uint64_t * _array = array;
		return [f, tc, _array, size, arg1, arg2, arg3]() {
			       f(tc, _array, size, arg1, arg2, arg3);
		       };
	}
	~one_array_2arg() {
		delete [] array;
	}
};


class raw_bytes: public benchmark_env<uint64_t*(*)(uint64_t *, unsigned long int)> {
	uint8_t * array;
	uint64_t max_size;
public:
	raw_bytes(uint64_t max_size): max_size(max_size) {
		array =  new uint8_t[max_size];
	}
	
	void reset_environment(const parameter_map_t & parameters) {
		uint64_t a = 1;
		uint64_t * t = reinterpret_cast<uint64_t*>(array);
		
		for (unsigned int i = 0; i < max_size/sizeof(uint64_t); i++) {
			t[i] = fast_rand(&a);
		}
	}
	void sort_data(parameter_map_t & parameters) {
	        uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
	        std::sort(array, array+size);	
	}
	
	std::function<void()> get_function(void * the_func, parameter_map_t & parameters) {
		uint64_t size = boost::any_cast<uint64_t>(parameters["size"]);
		auto f = cast_function(the_func);
		uint64_t * _array = reinterpret_cast<uint64_t*>(array);
		return [f, _array, size]() {
			       f(_array, size);
		       };
	}
	~raw_bytes() {
		delete [] array;
	}
};

int main(int argc, char *argv[])
{

	
	std::vector<int> mhz_s;
	std::vector<int> default_mhz;
	std::vector<int> sort;
	std::vector<int> default_sort;
	std::vector<unsigned long int> threads;
	std::vector<unsigned long int> default_threads;
	std::vector<unsigned long int> sizes;
	std::vector<unsigned long int> default_sizes;
	int i, reps=1, size, iterations=1,mhz, arg1, perf_detail_level=0;
        char *stat_file = NULL;
        char default_filename[] = "stats.csv";
        char preamble[1024];
        char preamble_input[1024] ="";
        char epilogue[1024];
        char header[1024];
	
	default_mhz.push_back(3600);

	std::vector<std::string> functions;
	std::vector<std::string> default_functions;
	std::vector<std::string> libs;
	std::vector<std::string> default_libs;
	// std::stringstream available_functions;
	// for(auto & f: function_map::get()) {
	// 	available_functions << "'" << f.first << "' ";
	// }
	default_sizes.push_back(1024*1024);
	std::vector<unsigned long int> sizes2;
	std::vector<unsigned long int> default_sizes2;
	default_sizes2.push_back(1024*16);
	std::vector<unsigned long int> sizes3;
	std::vector<unsigned long int> default_sizes3;
	default_sizes3.push_back(1024*1024);
 	std::vector<unsigned long int> arg1s;
	std::vector<unsigned long int> default_arg1s;
 	std::vector<unsigned long int> arg2s;
	std::vector<unsigned long int> default_arg2s;
 	std::vector<unsigned long int> arg3s;
	std::vector<unsigned long int> default_arg3s;
	default_arg1s.push_back(1);
	default_arg2s.push_back(1);
	default_arg3s.push_back(1);
	default_threads.push_back(1);
	std::vector<int32_t> tile_sizes;
	std::vector<int32_t> default_tile_sizes;
	default_sort.push_back(0);
	default_tile_sizes.push_back(64);
        for(i = 1; i < argc; i++)
        {
            // This is an option.
            if(argv[i][0]=='-')
            {
                switch(argv[i][1])
                {
                    case 'o':
		        if(i+1 < argc && argv[i+1][0]!='-')
                            stat_file = argv[i+1];
                        break;
                    case 'r':
		        if(i+1 < argc && argv[i+1][0]!='-')
                            reps = atoi(argv[i+1]);
                        break;
		    case 's':
		        for(;i+1<argc;i++)
		        {
		            if(argv[i+1][0]!='-')
		            {
		                size = atoi(argv[i+1]);
	                        sizes.push_back(size);
			    }
			    else
			        break;
		        }
		        break;
		    case 'M':
		        for(;i+1<argc;i++)
		        {
		            if(argv[i+1][0]!='-')
		            {
		                mhz = atoi(argv[i+1]);
	                        mhz_s.push_back(mhz);
			    }
			    else
			        break;
		        }
		        break;
		    case 'a':
		        for(;i+1<argc;i++)
		        {
		            if(argv[i+1][0]!='-')
		            {
		                arg1 = atoi(argv[i+1]);
	                        arg1s.push_back(arg1);
			    }
			    else
			        break;
		        }
		        break;
		    case 'f':
		        for(;i+1<argc;i++)
		        {
		            if(argv[i+1][0]!='-')
		            {
	                            functions.push_back(std::string(argv[i+1]));
			    }
			    else
			        break;
		        }
		        break;
		    case 'l':
		        for(;i+1<argc;i++)
		        {
		            if(argv[i+1][0]!='-')
		            {
	                            libs.push_back(std::string(argv[i+1]));
			    }
			    else
			        break;
		        }
		        break;
		    case 'i':
		        if(i+1 < argc && argv[i+1][0]!='-')
                            iterations = atoi(argv[i+1]);
		        break;
		    case '-':
		        if(strcmp(&argv[i][2], "sort") == 0)
		        {
			     sort.push_back(0);
			     sort.push_back(1);
		             std::cerr << "Will sort input" << "\n";
		        }
		        else if(strcmp(&argv[i][2], "detail") == 0)
		        {
		             perf_detail_level = 1;
		        }
		        else if(strcmp(&argv[i][2], "header") == 0)
		        {
			     strcpy(header, argv[i+1]);
		        }
		        else if(strcmp(&argv[i][2], "preamble") == 0)
		        {
			     strcpy(preamble_input, argv[i+1]);
		        }
		        else if(strcmp(&argv[i][2], "arg1") == 0)
		        {
		            for(;i+1<argc;i++)
		            {
		                if(argv[i+1][0]!='-')
		                {
		                    arg1 = atoi(argv[i+1]);
	                            arg1s.push_back(arg1);
			        }
			        else
			            break;
		             }
		             break;
		        }
		        else if(strcmp(&argv[i][2], "arg2") == 0)
		        {
		            for(;i+1<argc;i++)
		            {
		                if(argv[i+1][0]!='-')
		                {
		                    arg1 = atoi(argv[i+1]);
	                            arg2s.push_back(arg1);
			        }
			        else
			            break;
		             }
		             break;
		        }
		        else if(strcmp(&argv[i][2], "arg3") == 0)
		        {
		            for(;i+1<argc;i++)
		            {
		                if(argv[i+1][0]!='-')
		                {
		                    arg1 = atoi(argv[i+1]);
	                            arg3s.push_back(arg1);
			        }
			        else
			            break;
		             }
		             break;
		        }
		        else if(strcmp(&argv[i][2], "size") == 0)
		        {
		            for(;i+1<argc;i++)
		            {
		                if(argv[i+1][0]!='-')
		                {
		                    size = atoi(argv[i+1]);
	                            sizes.push_back(size);
			        }
			        else
			            break;
		             }
		             break;
		        }
		        else if(strcmp(&argv[i][2], "size2") == 0)
		        {
		            for(;i+1<argc;i++)
		            {
		                if(argv[i+1][0]!='-')
		                {
		                    size = atoi(argv[i+1]);
	                            sizes2.push_back(size);
			        }
			        else
			            break;
		             }
		             break;
		        }
		        else if(strcmp(&argv[i][2], "size3") == 0)
		        {
		            for(;i+1<argc;i++)
		            {
		                if(argv[i+1][0]!='-')
		                {
		                    size = atoi(argv[i+1]);
	                            sizes3.push_back(size);
			        }
			        else
			            break;
		             }
		             break;
		        }
		        else if(strcmp(&argv[i][2], "threads") == 0)
		        {
		            for(;i+1<argc;i++)
		            {
		                if(argv[i+1][0]!='-')
		                {
	                            threads.push_back(atoi(argv[i+1]));
			        }
			        else
			            break;
		             }
		             break;
		        }
		        break;
		    case 'h':
		        break;
                }
            }
        }
	if(stat_file==NULL)
	    stat_file = default_filename;



	for(auto & l: libs) {
		void * lib = dlopen(l.c_str(), RTLD_LOCAL|RTLD_NOW);
		if (lib == NULL){
			std::cerr << "Couldn't loadlib " << dlerror() << "\n";
			exit(1);
		}
		void (*f)(function_map_t &) = (void (*)(function_map_t &))dlsym(lib, "register_functions");
		if (f == NULL) {
			std::cerr << "Couldn't load function " << dlerror() << "\n";
			exit(1);
		}
		f(function_map::get());
	}

	if(sizes.size()==0)
	    sizes = default_sizes;
	if(mhz_s.size()==0)
	    mhz_s = default_mhz;
	if(functions.size()==0)
	    functions = default_functions;
	if(sizes.size()==0)
	    sizes = default_sizes;
	if(sizes2.size()==0)
	    sizes2 = default_sizes2;
	if(arg1s.size()==0)
	    arg1s = default_arg1s;
	if(arg2s.size()==0)
	    arg2s = default_arg2s;
	if(arg3s.size()==0)
	    arg3s = default_arg3s;
	if(threads.size()==0)
	    threads = default_threads;
	if(sort.size()==0)
	    sort = default_sort;
       
	REGISTER_ENV(one_array_1arg, new one_array_1arg(*std::max_element(sizes.begin(), sizes.end())));
	REGISTER_ENV(one_array_2arg, new one_array_2arg(*std::max_element(sizes.begin(), sizes.end())));
	REGISTER_ENV(one_array, new one_array(*std::max_element(sizes.begin(), sizes.end())));
	REGISTER_ENV(two_arrays, new two_arrays(*std::max_element(sizes.begin(), sizes.end())));
	REGISTER_ENV(three_arrays, new three_arrays(*std::max_element(sizes.begin(), sizes.end())));
	REGISTER_ENV(convolution, new convolution(*std::max_element(sizes.begin(), sizes.end())));
	REGISTER_ENV(raw_bytes, new raw_bytes(*std::max_element(sizes.begin(), sizes.end())));
	REGISTER_ENV(alloc_test, new alloc_test);

//	theDataCollector->disable_prefetcher();

	if (std::find(functions.begin(), functions.end(), "ALL") != functions.end()) {
		functions.clear();
		for(auto & f : function_map::get()) {
			functions.push_back(f.first);
		}
	}
	
	for(auto & function : functions) {
		auto t= function_map::get().find(function);
		if (t == function_map::get().end()) {
			std::cerr << "Unknown function: " << function <<"\n";
			exit(1);
		}
		auto s = benchmark_env_map::get().find(t->second.first);
		if ( s == benchmark_env_map::get().end()) {
			std::cerr << "Unknown benchmark env: " << t->second.first << " for " << function <<"\n";
			exit(1);
		}
	}
	std::cout << "Execution started\n" ;
	
	sprintf(header,"%sreps,size,size2,threads,arg1,arg2,arg3,function,IC,Cycles,CPI,CT,ET,L1_dcache_miss_rate,L1_dcache_misses,L1_dcache_accesses,branches,branch_misses",header);
        perfstats_print_header(stat_file, header);
	parameter_map_t params;
	for(auto &mhz: mhz_s) 
        {
	    change_cpufrequnecy(mhz);
	    for(auto & thread: threads ) 
	    {
		omp_set_num_threads(thread);
		params["thread"] = thread;
		for(auto & arg1: arg1s ) 
		{
			//std::cout <<"arg1:" <<arg1;
			params["arg1"] = arg1;
		        for(auto & arg2: arg2s ) 
		//	for(auto & tile_size: tile_sizes ) 
		        {
				params["arg2"] = arg2;
		            for(auto & arg3: arg3s ) 
		            {
				params["arg3"] = arg3;
				for(auto & size: sizes ) 
				{
					params["size"] = size;
//					std::cout <<"size:" <<size;
					for(auto & size2: sizes2 ) 
					{
						params["size2"] = size2;
					
						//for(auto & sorting: sort ) 
						{
							//params["size3"] = size3;
							for(auto & function : functions) {
								//START_TRACE();
								//std::cerr << "Running " << function;
								function_spec_t f_spec = function_map::get()[function];
								auto env = benchmark_env_map::get()[f_spec.first];
								auto fut = env->get_function(f_spec.second, params);
								for(uint r = 0; r < reps; r++) {
									env->reset_environment(params);
//									if(sorting == 1)
//									    env->sort_data(params);
									//std::cerr << function.c_str() <<"."<< arg1<< "\n";
									{
										sprintf(preamble, "%s%d,%lu,%lu,%lu,%lu,%lu,%lu,%s,",preamble_input,reps,size,size2,thread,arg1,arg2,arg3,function.c_str());
										perfstats_init();
										perfstats_enable(perf_detail_level);

										for(unsigned int i =0; i < iterations; i++) {
											fut();
										}
										perfstats_disable(perf_detail_level);
										sprintf(epilogue,"\n");
										perfstats_print(preamble, stat_file, epilogue);
										perfstats_deinit();
									}								
								}
//								std::cerr << "\n";
							}
						}
					}
				}
			    }
			}
		}
	    }
        restore_cpufrequnecy();
	}
	std::cout << "Execution completed\n" ;
//	archlab_write_stats();
	return 0;
}
