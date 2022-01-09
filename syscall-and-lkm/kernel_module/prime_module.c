#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> 
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/ktime.h>

int n, i, j, n_prime;
unsigned int* buff_Ns;
unsigned int rand;
ktime_t start_time, stop_time, elapsed_time;

module_param(n, int, 0);
MODULE_PARM_DESC(n, "The number of random numbers");

static int is_prime(int num){
    if (num <= 1) return 0;
    for (j=2; j<=num/2; j++) {
        if(num % j == 0) return 0;
    }
    return 1;
}

static int __init lkp_init(void){
    printk(KERN_INFO "The number of total random numbers : %d\n", n);

    // kmalloc
    buff_Ns = kmalloc(sizeof(unsigned int) * n, GFP_KERNEL);

    // generate radom numbers
    for(i=0; i<n; i++){
        get_random_bytes(&rand, sizeof(unsigned int));
        buff_Ns[i] = rand % n + 1;
    }

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
    printk(KERN_INFO "Processing time : %lld us\n", ktime_to_ns(elapsed_time) / 1000);

    // kfree
    kfree(buff_Ns);

    return 0 ; 
}

static void __exit lkp_exit(void){
    printk(KERN_INFO "Module exiting...\n");
}

module_init(lkp_init);
module_exit(lkp_exit);
MODULE_LICENSE("GPL");

