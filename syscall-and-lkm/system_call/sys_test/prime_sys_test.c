#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>

#define __PRIME_SYS 549 // 549 is defined number in syscall_64.tbl

int main(int argc, char** argv){
    // get user input
    if(argc != 3){
		printf("Usage : %s -n <# of random numbers>\n", argv[0]);
		return 0;
    }
    int n = atoi(argv[2]);

    // malloc array
	unsigned int* Ns = (unsigned int*)malloc(sizeof(unsigned int) * n);

    // generate random numbers
    srand(n);
    for(int i=0; i<n; i++) Ns[i] = rand() % n + 1;

    // syscall
	syscall(__PRIME_SYS, Ns, n);

    // free array
    free(Ns);

	return 0;
}
