all: virtualization memory_allocation 

virtualization: virtualization.c
	gcc -O3 virtualization.c -o virtualization

memory_allocation: memory_allocation.c
	gcc -O3 memory_allocation.c -o memory_allocation
#	objdump -D virtualization > virtualization.dump
clean:
	rm -f virtualization memory_allocation 
