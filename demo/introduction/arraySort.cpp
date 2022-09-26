#include <algorithm>
#include <ctime>
#include <iostream>
#include <climits>

long long calculate_sum(int *data, unsigned int size, int threshold);

int main(int argc, char **argv)
{
    // generate data
//    const unsigned arraySize = 262144;
    int arraySize = atoi(argv[1]);
    int data[arraySize];
    long long sum = 0;

    int iterations = atoi(argv[2]);
    int option = atoi(argv[3]);

    for (unsigned c = 0; c < arraySize; ++c)
        data[c] = std::rand();

    if(option)
        std::sort(data, data + arraySize);

    for (unsigned i = 0; i < iterations; ++i) {
        sum += calculate_sum(data, arraySize, std::rand());
    }
    std::cout << "sum = " << sum << std::endl;
}

long long calculate_sum(int *data, unsigned int size, int threshold)
{
    long long sum=0;
    for (unsigned i = 0; i < size; ++i) {
        if (data[i] >= threshold)
                sum ++;
    }
    return sum;
}