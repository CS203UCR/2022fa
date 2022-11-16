#include<iostream>
#include<assert.h>
#include <stdint.h>
#include <stdio.h>
#ifdef SSE42
#include <smmintrin.h>
inline int popcount(uint64_t x) {
     int c = _mm_popcnt_u64(x);
     return c;
}
#else

#ifdef C
inline int popcount(uint64_t x) {
     int c = 0;
     int table[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
     while(x) {
	       c += table[(x & 0xF)];
	       x = x >> 4;
     }
     return c;
}

#else
#ifdef B
inline int popcount(uint64_t x) {
     int c = 0;
	 while(x) //while any bits are set not sure how you are getting the value
     {
       c += x & 1;
       x = x >> 1;
       c += x & 1;
       x = x >> 1;
       c += x & 1;
       x = x >> 1;
       c += x & 1;
       x = x >> 1;
     }
     return c;
}
#else
#ifdef D
inline int popcount(uint64_t x) {
     int c = 0;
     int table[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
     for (uint64_t i = 0; i < 16; i++) {
	       c += table[(x & 0xF)];
	       x = x >> 4;
     }
     return c;
}
#else 
#ifdef E
inline int popcount(uint64_t x) {
     int c = 0;
     for (uint64_t i = 0; i < 16; i++) {
         switch((x & 0xF))
         {
             case 1: c+=1; break;
             case 2: c+=1; break;
             case 3: c+=2; break;
             case 4: c+=1; break;
             case 5: c+=2; break;
             case 6: c+=2; break;
             case 7: c+=3; break;
             case 8: c+=1; break;
             case 9: c+=2; break;
             case 10: c+=2; break;
             case 11: c+=3; break;
             case 12: c+=2; break;
             case 13: c+=3; break;
             case 14: c+=3; break;
             case 15: c+=4; break;
             default: break;
         }
         x = x >> 4;
     }
     return c;
}
#else //A
inline int popcount(uint64_t x){
	int c=0;
	while(x) //while any bits are set not sure how you are getting the value
    {
       c += x & 1;
       x = x >> 1;
    }
    return c;
}
#endif
#endif
#endif
#endif
#endif// Simple, very fast pseudo-random number generator
uint64_t RandLFSR(uint64_t &seed) {
     uint64_t bit;
     uint64_t temp = seed;
     bit = ((temp >> 0) ^ (temp >> 1) ^ (temp >> 3) ^ (temp >> 4)) & 0x1llu;
     seed = (temp >> 1) | (bit << 63);
     return seed;
}

int identity(uint64_t x) {
     return x;
}

int main(int argc, char *argv[]) {

     uint64_t key = 0xdeadbeef;

     int count = 1000000000;
     uint64_t sum = 0;
     
     for (int i=0; i < count; i++)
     { 
	   sum += popcount (RandLFSR(key)); 
     }
     printf("Result: %lu\n", sum);
     return sum;
}
