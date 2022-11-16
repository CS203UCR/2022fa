import numpy as np
import sys
import time

n = len(sys.argv)
method = 0;
if n > 1:
    method = sys.argv[0]
    
data = np.random.randint(1,26,10000000)
sum_1 = 0;
sum_2 = 0;
tic = time.perf_counter()
if method == 1:
    for x in data: 
        if x % 2 == 0: 
            sum_2 += x
        if x % 4 == 0:
            sum_1 += x
else:
    for x in data: 
        if x % 4 == 0:
            sum_1 += x
        if x % 2 == 0: 
            sum_2 += x
toc = time.perf_counter()
print(f"The loop in {toc - tic:0.4f} seconds")
print (" sum_1: ", sum_1);
print (" sum_2: ", sum_2);
