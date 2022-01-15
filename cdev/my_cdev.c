/*
 * Skeleton developed by UPB, modified for Ajou Univ. SCE394.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
/* TODO: add header files to complete thie module */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#include "my_cdev.h"

MODULE_DESCRIPTION("SCE394 character device");
MODULE_AUTHOR("SCE394");
MODULE_LICENSE("GPL");

#ifndef BUFSIZ
#define BUFSIZ		4096
#endif

#define MY_MAJOR	42
#define MY_MAX_MINORS 	5

struct my_device_data {
	/* TODO: add cdev member */
	struct cdev cdev;
	/* TODO: add buffer with BUFSIZ elements */
	char buffer[BUFSIZ];
	/* TODO: add read/write buffer size */
	size_t size;
	/* TODO: add atomic_t access variable to keep track if file is opened */
	atomic_t access;
};

struct my_device_data devs[MY_MAX_MINORS];

static size_t my_min(size_t n1, size_t n2) {
	if (n1 <= n2)
		return n1;
	else
		return n2;
}

static int my_open(struct inode *inode, struct file *file)
{
	struct my_device_data *my_data;

	/* TODO: print message when the device file is open. */
	printk(KERN_INFO "%s opened\n", MODULE_NAME);

	/* TODO: inode->i_cdev contains our cdev struct, use container_of to obtain a pointer to my_device_data */
	my_data = container_of(inode->i_cdev, struct my_device_data, cdev);
	file->private_data = my_data;

	/* TODO: return immediately if access is != 0, use atomic_cmpxchg */
	if(atomic_cmpxchg(&my_data->access, 0, 1) != 0)
		return -EBUSY;

	/*
	 *  If you want to test open after relase device file, 
	 *  use below functions to sleep open().
	 */
	// set_current_state(TASK_INTERRUPTIBLE);
	// schedule_timeout(10 * HZ);

	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	struct my_device_data *my_data =
		(struct my_device_data *) file->private_data;
	/* TODO: print message when the device file is closed. */
	printk(KERN_INFO "%s released\n", MODULE_NAME);

	/* TODO: reset acces varable to 0, use atomic_set */
	atomic_set(&my_data->access, 0);

	return 0;
}

static ssize_t my_read(struct file *file,
		char __user *user_buffer,
		size_t size, loff_t *offset)
{
	struct my_device_data *my_data;
	size_t len;

	my_data = (struct my_device_data*) file->private_data;

	printk(KERN_INFO "%s read() called\n", MODULE_NAME);

	len = my_min(my_data->size - *offset, size);
	if(len <= 0)
		return 0;

	/* TODO: Copy my_data->buffer to user_buffer, use copy_to_user */
	if(copy_to_user(user_buffer, my_data->buffer + *offset, len)) 
		return -EFAULT;
	
	*offset += len;
	return len;
}

static ssize_t my_write(struct file *file,
		const char __user *user_buffer,
		size_t size, loff_t *offset)
{
	struct my_device_data *my_data;
	my_data = (struct my_device_data*) file->private_data;

	printk(KERN_INFO "%s write() called\n", MODULE_NAME);

	/* TODO: copy user_buffer to my_data->buffer, use copy_from_user */
	if(copy_from_user(my_data->buffer + *offset, user_buffer, size))
		return -EFAULT;
	my_data->buffer[size+1] = '\0';

	*offset += size;
	return size;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct my_device_data *my_data = (struct my_device_data *) file->private_data;
	int ret = 0;
	int remains;

	switch (cmd) {
		/* TODO: if cmd = MY_IOCTL_PRINT, display IOCTL_MESSAGE */
		case MY_IOCTL_PRINT:
			printk(KERN_INFO "%s \n", IOCTL_MESSAGE);
			break;

		/* TODO: if cmd = MY_IOCTL_SET_BUFFER, write arg to my_data->buffer */
		case MY_IOCTL_SET_BUFFER:
			remains = copy_from_user(my_data->buffer, (char __user*) arg, BUFFER_SIZE);
			if(remains)
				ret = -EFAULT;
			my_data->size = BUFFER_SIZE - remains;
			break;

		/* TODO: if cmd = MY_IOCTL_GET_BUFFER, read my_data->buffer to arg */
		case MY_IOCTL_GET_BUFFER:
			if(copy_to_user((char __user*)arg, my_data->buffer, my_data->size))
				ret = -EFAULT;
			break;

		default:
			ret = -EINVAL;
	}

	return ret;
}

static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	/* TODO: add open and release functions */
	.open = my_open,
	.release = my_release,
	/* TODO: add read function */
	.read = my_read,
	/* TODO: add write function */
	.write = my_write,
	/* TODO: add ioctl function */
	.unlocked_ioctl = my_ioctl
};

static int __init my_init_module(void)
{
	int err;
	int i;

	/* TODO: register char device region for MY_MAJOR and MY_MAX_MINORS starting at 0 */
	err = register_chrdev_region(MKDEV(MY_MAJOR, 0), MY_MAX_MINORS, "my_device_driver");
	if(err != 0){
		return err;
	}

	for (i=0; i<MY_MAX_MINORS; i++) {
		/* TODO: init and add cdev to kernel core */
		cdev_init(&devs[i].cdev, &my_fops);
		cdev_add(&devs[i].cdev, MKDEV(MY_MAJOR, i), 1);
		/* TODO: set access variable to 0, use atomic_set */
		atomic_set(&devs[i].access, 0);
		/* TODO: initialize buffer with MESSAGE string */
		devs[i].size = BUFSIZ;
		strncpy(devs[i].buffer, MESSAGE, devs[i].size);
	}

	printk(KERN_INFO "%s loaded\n", MODULE_NAME);
	return 0;
}

static void __exit my_cleanup_module(void)
{
	int i;
	/* TODO: delete cdev from kernel core */
	for(i=0; i<MY_MAX_MINORS; i++){
		cdev_del(&devs[i].cdev);
	}
	unregister_chrdev_region(MKDEV(MY_MAJOR, 0), MY_MAX_MINORS);

	/* TODO: unregister char device region, for MY_MAJOR and MY_MAX_MINORS starting at 0 */
	printk(KERN_INFO "%s unloaded\n", MODULE_NAME);
}

module_init(my_init_module);
module_exit(my_cleanup_module);
