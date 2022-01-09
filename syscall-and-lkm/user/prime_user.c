#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

static int is_prime(int num){
    if (num <= 1) return 0;
    for (int j=2; j<=num/2; j++) {
        if(num % j == 0) return 0;
    }
    return 1;
}

int main(int argc, char** argv){
    struct timeval start_time, end_time;
    long diff_time;

    // get user input
    if(argc != 3){
		printf("Usage : %s -n <# of random numbers>\n", argv[0]);
		return 0;
    }
    int n = atoi(argv[2]);
    printf("The number of total random numbers : %d\n", n);

    // kmalloc
    unsigned int* Ns = (unsigned int*)malloc(sizeof(unsigned int) * n);

    // generate random numbers
    srand(n);
    for(int i=0; i<n; i++) Ns[i] = rand() % n + 1;

    // time measure (1)
    gettimeofday(&start_time, NULL);

    // check prime
    int n_prime = 0;
    for(int i=0; i<n; i++){
        if(is_prime(Ns[i])) {
            n_prime++;
        }
    }
    
    // time measure (2)
    gettimeofday(&end_time, NULL);
    diff_time = end_time.tv_usec - start_time.tv_usec;

    // print result
    printf("The number of prime numbers : %d\n", n_prime);
    printf("Processing time : %ld us\n", diff_time);

    // buf_N : kfree
    free(Ns);

	return 0;
}
