all: popcount popcount_B popcount_C popcount_D popcount_SSE42 popcount_E

popcount: 
	g++ -O3 popcount.cpp -o popcount_A
	g++ -S -O3 popcount.cpp -o A.s

popcount_B: 
	g++ -O3 -DB popcount.cpp -o popcount_B
	g++ -S -O3 -DB popcount.cpp -o B.s

popcount_C: 
	g++ -O3 -DC popcount.cpp -o popcount_C
	g++ -S -O3 -DC popcount.cpp -o C.s

popcount_D: 
	g++ -O3 -DD popcount.cpp -o popcount_D
	g++ -S -O3 -DD popcount.cpp -o D.s

popcount_E: 
	g++ -O3 -DE popcount.cpp -o popcount_E
	g++ -S -O3 -DE popcount.cpp -o E.s

popcount_SSE42: 
	g++ -O3 -m64 -msse4.2 -DSSE42 popcount.cpp -o popcount_SSE42
	
dump:
	otool -tV popcount
	otool -tV popcount_inline 
	
clean:
		rm -f popcount_A popcount_B popcount_C popcount_D popcount_SSE42 popcount_E
		