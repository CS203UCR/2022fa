#ifndef FUNCTION_MAP_INCLUDED
#define FUNCTION_MAP_INCLUDED
#include<map>
#include <boost/any.hpp>
#include<functional>
#include<iostream>

// I played around with speeding up compilation by compiling the functions into
// .so files and loading them dynamically.  I ran into problems because this
// implementation of Singleton cannot be shared between the .so and the
// executable its linked to.  The .so and the .exe both get their own copies.
//
// Using a global to hold the function registry doesn't work reliably because
// it's hard to ensure that the global is initalized before the function is
// registered.
//
// I guess a solution would be to open the .so with dlopen, and the copy the
// contents of the .so's registry into the .exe's registry.  THe .exe could
// call some well-known function in the .so and pass in it's registry.  or
// something...

typedef std::pair<std::string, void*> function_spec_t;

template<class T>
class Singleton
{
private:
   Singleton();

public:
   static T& get()
   {
      static T INSTANCE;
      return INSTANCE;
   }
};


typedef std::map<const std::string, function_spec_t> function_map_t;
typedef Singleton<function_map_t> function_map;


extern "C"
void __attribute((weak)) register_functions(function_map_t & global_map) { 
	for(auto &p : function_map::get()) {
		std::cerr << "Dynamically registering " << p.first << "\n";
		global_map[p.first] = p.second;
	}
}

class RegisterFunction
{
public:
	RegisterFunction(std::string name, std::string kind, void*function) {
		std::cerr << "registering function: " << name << "\n";
		function_map::get()[name] = std::pair<std::string, void*>(kind, function);
	}
};

#define FUNCTION(kind, f) RegisterFunction f##_reg(#f, #kind, (void*)f)

/////////////////////////////////

typedef std::map<std::string, boost::any> parameter_map_t;

class benchmark_env_base {
public:
	virtual void reset_environment(const parameter_map_t & parameters) = 0;
	virtual void sort_data(parameter_map_t & parameters) = 0;
	virtual std::function<void()> get_function(void * the_func, parameter_map_t & parameters) = 0;
	virtual ~benchmark_env_base(){}
};

template<class F>
class benchmark_env: public benchmark_env_base {
public:
        F cast_function(void * function) {
		return (F)function;
	}
};


typedef Singleton<std::map<std::string, benchmark_env_base*> > benchmark_env_map;

class RegisterBenchmarkEnv
{
public:
	RegisterBenchmarkEnv(const std::string &  name, benchmark_env_base* env) {
		//std::cerr << "registering env: " << name << "\n";
		benchmark_env_map::get()[name] = env;
	}
};

#define REGISTER_ENV(name, env) RegisterBenchmarkEnv name##_reg(#name, env)

#endif
