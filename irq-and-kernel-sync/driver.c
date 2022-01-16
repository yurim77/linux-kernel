/***************************************************************************//**
 *  \file       driver.c
 *
 *  \details    Interrupt Example
 *
 *  \author     EmbeTronicX
 *
 *  \modifier   Jonghyeon Kim, Yurim Jang
 *
 *******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
// #include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
// #include<linux/slab.h>              // kmalloc()
#include<linux/uaccess.h>              // copy_to/from_user()
// #include<linux/sysfs.h> 
// #include<linux/kobject.h> 
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/hw_irq.h>

#include <linux/spinlock.h>

#include "driver.h"

#define IRQ_NO		11

#define MY_MAJOR	42
#define MY_MAX_MINORS	1

#define DEVICE		"mycdev"


/* TODO: use this lock or more lock variables you want */
static spinlock_t lock_s;

// TODO: To raise IRQ, you must modify kernel codes
// Add the EXPORT_SYMBOL_GPL(vector_irq) after #include definition in arch/x86/kernel/irq.c
// Add the EXPORT_SYMBOL_GPL(irq_to_desc) after irq_to_desc() in kernel/irq/irqdesc.c
// After then, we need to rebuild the linux source: make -j$(nproc)

struct my_device_data {
	struct cdev cdev;
	int num[5];
	int sum;
};

struct my_device_data dev_data;

static int __init my_driver_init(void);
static void __exit my_driver_exit(void);

/*************** Driver Fuctions **********************/
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, 
		char __user *buf, size_t len,loff_t * off);
static ssize_t my_write(struct file *file, 
		const char *buf, size_t len, loff_t * off);
static ssize_t my_ioctl(struct file *file, 
		unsigned int cmd, unsigned long arg);

static struct file_operations fops =
{
	.owner          = THIS_MODULE,
	.read           = my_read,
	.write          = my_write,
	.open           = my_open,
	.release        = my_release,
	.unlocked_ioctl = my_ioctl
};

// Interrupt handler for IRQ 11. 
static irqreturn_t irq_handler(int irq, void *dev_id) {
	int i = 0;

	/* TODO: if you need lock, implement */
    spin_lock(&lock_s);
	dev_data.num[0] = -1;
	dev_data.num[1] = -2;
	dev_data.num[2] = -3;
	dev_data.num[3] = -4;
	dev_data.num[4] = -5;
	for (i = 0; i < 5; i ++)
    	dev_data.sum += dev_data.num[i];
    
	printk(KERN_INFO "IRQ, sum: %d\n", dev_data.sum);
    spin_unlock(&lock_s);

	return IRQ_HANDLED;
}

static int my_open(struct inode *inode, struct file *file)
{
	struct my_device_data *my_data;
	my_data = container_of(inode->i_cdev, struct my_device_data, cdev);
	file->private_data = my_data;
	printk(KERN_INFO "Device File Opened...!!!\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device File Closed...!!!\n");
	return 0;
}

static ssize_t my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	struct irq_desc *desc;
	struct my_device_data *my_data = (struct my_device_data *) file->private_data;
	int ret = 0;
	int sum = 0;
	int i;
    unsigned long flags;

	/* TODO: if you need spinlock or mutex, use them */
	switch (cmd) {
		case MY_IOCTL_PRINT0:
            spin_lock_irqsave(&lock_s, flags);
			printk(KERN_INFO "IOCTL0, sum: %d\n", my_data->sum);
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		case MY_IOCTL_PRINT1:
            spin_lock_irqsave(&lock_s, flags);
			printk(KERN_INFO "IOCTL1, sum: %d\n", my_data->sum);
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		case MY_IOCTL_PRINT2:
            spin_lock_irqsave(&lock_s, flags);
			printk(KERN_INFO "IOCTL2, sum: %d\n", my_data->sum);
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		case MY_IOCTL_INC_0:
            spin_lock_irqsave(&lock_s, flags);
			my_data->num[0]++;
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		case MY_IOCTL_INC_1:
            spin_lock_irqsave(&lock_s, flags);
			my_data->num[1]++;
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		case MY_IOCTL_INC_2:
            spin_lock_irqsave(&lock_s, flags);
			my_data->num[2]++;
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		case MY_IOCTL_INC_3:
            spin_lock_irqsave(&lock_s, flags);
			my_data->num[3]++;
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		case MY_IOCTL_INC_4:
            spin_lock_irqsave(&lock_s, flags);
			my_data->num[4]++;
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		case MY_IOCTL_INC_ALL:
            spin_lock_irqsave(&lock_s, flags);
			my_data->num[0]++;
			my_data->num[1]++;
			my_data->num[2]++;
			my_data->num[3]++;
			my_data->num[4]++;
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		case MY_IOCTL_INT:
			/* raise interrupt IRQ_NO(11) */
			desc = irq_to_desc(IRQ_NO);
			if (!desc) {
				return -EINVAL;
			}
			__this_cpu_write(vector_irq[59], desc);
			asm("int $0x3B");  // Corresponding to irq 11
			break;
		case MY_IOCTL_SUM:
            spin_lock_irqsave(&lock_s, flags);
			for (i = 0; i < 5; i++)
				sum += my_data->num[i];
			my_data->sum = sum;
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		case MY_IOCTL_RESET:
            spin_lock_irqsave(&lock_s, flags);
			my_data->num[0] = 0;
			my_data->num[1] = 0;
			my_data->num[2] = 0;
			my_data->num[3] = 0;
			my_data->num[4] = 0;
			my_data->sum = 0;
            spin_unlock_irqrestore(&lock_s, flags);
			break;
		default:
			ret = -EINVAL;
	}
	return ret;
}

static ssize_t my_read(struct file *file, 
		char __user *buf, size_t size, loff_t *off)
{
	printk(KERN_INFO "Read function\n");
	return size;
}

static ssize_t my_write(struct file *file, 
		const char __user *buf, size_t size, loff_t *off)
{
	printk(KERN_INFO "Write Function\n");
	return size;
}

static int __init my_driver_init(void)
{
	int i;
    int err;

	/* Registering characther device to the system */
	if (register_chrdev_region(MKDEV(MY_MAJOR, 0), MY_MAX_MINORS, DEVICE)) {
		printk(KERN_INFO "Cannot register the device to the system\n");
		return -ENODEV;
	}

	/* Creating cdev structure */
	cdev_init(&dev_data.cdev, &fops);

	/* Adding character device to the system */
	if ((cdev_add(&dev_data.cdev, MKDEV(MY_MAJOR, 0) ,1)) < 0){
		printk(KERN_INFO "Cannot add the device to the system\n");
		return -EBUSY;
	}

	/* initialize attributes of struct my_device_data */
	for (i = 0; i < 5; i ++)
		dev_data.num[i] = 0;

	dev_data.sum= 0;

	/* TODO: you need to register irq number, handler */
	/* TODO: init lock */
    err = request_irq(IRQ_NO, irq_handler, IRQF_ONESHOT, "irq_handler is called!\n", (void *)(irq_handler));
	if (err < 0) return err;
	spin_lock_init(&lock_s);

	printk(KERN_INFO "Device Driver Insert...Done!!!\n");
	return 0;
}

static void __exit my_driver_exit(void)
{
	/* TODO: you need to free irq, handler */
    free_irq(IRQ_NO, (void *)(irq_handler));
	cdev_del(&dev_data.cdev);
	unregister_chrdev_region(MKDEV(MY_MAJOR, 0), MY_MAX_MINORS);
	printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YURIM JANG");
MODULE_DESCRIPTION("A simple device driver - Interrupts");
MODULE_VERSION("1.9");
