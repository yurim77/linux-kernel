#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/ktime.h>

int i, j, n_prime;
unsigned int* buff_Ns;
unsigned long ret;

ktime_t start_time, stop_time, elapsed_time;

static int is_prime(int num){
    if (num <= 1) return 0;
    for (j=2; j<=num/2; j++) {
        if(num % j == 0) return 0;
    }
    return 1;
}

SYSCALL_DEFINE2(prime_sys, int*, Ns, int, n) {
    printk(KERN_INFO "The number of total random numbers : %d\n", n);

    // kmalloc
    buff_Ns = kmalloc(sizeof(unsigned int) * n, GFP_KERNEL);

    // copy data from N (user)
    ret = copy_from_user(buff_Ns, Ns, sizeof(int) * n);

    // time measure (1)
    start_time = ktime_get();

    // check prime
    n_prime = 0;
    for(i=0; i<n; i++){
        if(is_prime(buff_Ns[i])) {
            n_prime++;
        }
    }
    
    // time measure (2)
    stop_time = ktime_get();
    elapsed_time= ktime_sub(stop_time, start_time);

    // print result
    printk(KERN_INFO "The number of prime numbers : %d\n", n_prime);
    printk(KERN_INFO "Processing time : %lld us\n", ktime_to_ns(elapsed_time)/1000);

    // buf_N : kfree
    kfree(buff_Ns);

	return 0;
}
