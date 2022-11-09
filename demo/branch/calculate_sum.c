#ifdef __cplusplus
extern "C" {
#endif
long long calculate_sum(int *data, unsigned int size, int threshold)
{
    long long sum=0;
    for (unsigned i = 0; i < size; ++i) {
        if (data[i] >= threshold)
                sum ++;
    }
    return sum;
}
#ifdef __cplusplus
}
#endif
