#include <algorithm>
#include <ctime>
#include <iostream>
#include <climits>

#ifdef __cplusplus
extern "C" {
#endif
#include "perfstats.h"
long long calculate_sum(int *data, unsigned int size, int threshold);
#ifdef __cplusplus
}
#endif


int main(int argc, char **argv)
{
    // generate data
//    const unsigned arraySize = 262144;
    int arraySize = atoi(argv[1]);
    int data[arraySize];
    long long sum = 0;
        char preamble[1024];
        char epilogue[1024];
        char header[1024];
        char stat_file[] = "stats.csv";

    int iterations = atoi(argv[2]);
    int option = atoi(argv[3]);

    for (unsigned c = 0; c < arraySize; ++c)
        data[c] = std::rand();


    //START
    if(option)
        std::sort(data, data + arraySize);
    perfstats_init();
    perfstats_enable(1);
    for (unsigned i = 0; i < iterations; ++i) {
        sum += calculate_sum(data, arraySize, std::rand());
    }
    //END
    perfstats_disable(1);
    sprintf(epilogue,"\n");
    sprintf(preamble,"");
    perfstats_print(preamble, stat_file, epilogue);
    perfstats_deinit();
    std::cout << "sum = " << sum << std::endl;
}

