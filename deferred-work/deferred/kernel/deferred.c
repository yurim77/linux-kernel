#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched/task.h>
#include "../include/deferred.h"

#define MY_MAJOR		42
#define MY_MINOR		0
#define MODULE_NAME		"deferred"

#define TIMER_TYPE_NONE		-1
#define TIMER_TYPE_SET		0
#define TIMER_TYPE_ALLOC	1
#define TIMER_TYPE_MON		2

MODULE_DESCRIPTION("deferred work character device");
MODULE_AUTHOR("SCE393, yurim.github@gmail.com");
MODULE_LICENSE("GPL");

struct mon_proc {
	struct task_struct *task;
	struct list_head list;
};

static struct my_device_data {
	struct cdev cdev;
	/* add timer, flag, work,
	   list for monitored processes,
	   spinlock to protect list
	*/
	struct timer_list timer;
	int flag;
	struct work_struct work;
	struct list_head list;
	spinlock_t lock;
} dev;

static void alloc_io(void)
{
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(5 * HZ);
	pr_info("I've been sleeping for 5 seconds.\n");
}

static struct mon_proc *get_proc(pid_t pid)
{
	struct task_struct *task;
	struct mon_proc *p;

	rcu_read_lock();
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	rcu_read_unlock();
	if (!task)
		return ERR_PTR(-ESRCH);

	p = kmalloc(sizeof(*p), GFP_ATOMIC);
	if (!p)
		return ERR_PTR(-ENOMEM);

	get_task_struct(task);
	p->task = task;

	return p;
}


/* define work handler for alloc_io() */
static void work_handler(struct work_struct *work){
	alloc_io();
}

#define ALLOC_IO_DIRECT
#undef ALLOC_IO_DIRECT

static void timer_handler(struct timer_list *tl)
{
	/* implement timer handler */
	struct my_device_data *my_data = from_timer(my_data, tl, timer);
	pr_info("[timer_handler] pid = %d, comm = %s\n", current->pid, current->comm);
	
	/* check flags: TIMER_TYPE_SET or TIMER_TYPE_ALLOC */
	switch(my_data->flag){
		case TIMER_TYPE_SET:
			break;
		
		case TIMER_TYPE_ALLOC:
			/* schedule work */
#ifdef ALLOC_IO_DIRECT
			alloc_io();
#else
			schedule_work(&my_data->work);
#endif
			break;
		
		case TIMER_TYPE_MON:
		{
			/* iterate the list and check the proccess state */
			/* if task is dead, print info ... */
			/* ... decrement task usage counter ... */
			/* ... remove it from the list ... */
			/* ... free the struct mon_proc */
			struct mon_proc *p, *n;
			spin_lock(&my_data->lock);
			list_for_each_entry_safe(p, n, &my_data->list, list){
				if(p->task->__state == TASK_DEAD){
					pr_info("task %s (%d) is dead\n", p->task->comm, p->task->pid);
					put_task_struct(p->task);
					list_del(&p->list);
					kfree(p);
				}
			}
			spin_unlock(&my_data->lock);
			mod_timer(&my_data->timer, jiffies + HZ);
			break;
		}
		
		default:
			break;
	}
}

static int deferred_open(struct inode *inode, struct file *file)
{
	struct my_device_data *my_data =
		container_of(inode->i_cdev, struct my_device_data, cdev);
	file->private_data = my_data;
	pr_info("[deferred_open] Device opened\n");
	return 0;
}

static int deferred_release(struct inode *inode, struct file *file)
{
	pr_info("[deferred_release] Device released\n");
	return 0;
}

static long deferred_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct my_device_data *my_data = (struct my_device_data*) file->private_data;

	pr_info("[deferred_ioctl] Command: %s\n", ioctl_command_to_string(cmd));

	switch (cmd) {
		case MY_IOCTL_TIMER_SET:
			/* set flag */
			my_data->flag = TIMER_TYPE_SET;
			/* schedule timer */
			mod_timer(&my_data->timer, jiffies + arg * HZ);
			break;
		case MY_IOCTL_TIMER_CANCEL:
			/* cancel timer */
			del_timer(&my_data->timer);
			break;
		case MY_IOCTL_TIMER_ALLOC: // invoke alloc_io()
			/* set flag and schedule timer */
			my_data->flag = TIMER_TYPE_ALLOC;
			mod_timer(&my_data->timer, jiffies + arg * HZ);
			break;
		case MY_IOCTL_TIMER_MON:
		{
			/* use get_proc() and add task to list */
			struct mon_proc *p = get_proc(arg);
			if(IS_ERR(p))
				return PTR_ERR(p);
			/* protect access to list */
			spin_lock_bh(&my_data->lock);
			list_add(&p->list, &my_data->list);
			spin_unlock_bh(&my_data->lock);
			/* set flag and schedule timer */
			my_data->flag = TIMER_TYPE_MON;
			mod_timer(&my_data->timer, jiffies + HZ);
			break;
		}
		default:
			return -ENOTTY;
	}
	return 0;
}

struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = deferred_open,
	.release = deferred_release,
	.unlocked_ioctl = deferred_ioctl,
};

static int deferred_init(void)
{
	int err;

	pr_info("[deferred_init] Init module\n");
	err = register_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR), 1, MODULE_NAME);
	if (err) {
		pr_info("[deffered_init] register_chrdev_region: %d\n", err);
		return err;
	}

	dev.flag = TIMER_TYPE_NONE;
	
	INIT_WORK(&dev.work, work_handler);

	spin_lock_init(&dev.lock);
	INIT_LIST_HEAD(&dev.list);

	cdev_init(&dev.cdev, &my_fops);
	cdev_add(&dev.cdev, MKDEV(MY_MAJOR, MY_MINOR), 1);

	timer_setup(&dev.timer, timer_handler, 0);
	return 0;
}

static void deferred_exit(void)
{
	struct mon_proc *p, *n;

	pr_info("[deferred_exit] Exit module\n" );

	cdev_del(&dev.cdev);
	unregister_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR), 1);

	/* Cleanup: make sure the timer is not running after exiting. */
	del_timer_sync(&dev.timer);
	
	/* Cleanup: make sure the work handler is not scheduled. */
	flush_scheduled_work();

	/* Cleanup the monitered process list */
	list_for_each_entry_safe(p, n, &dev.list, list){
		put_task_struct(p->task);
		list_del(&p->list);
		kfree(p);
	}
}

module_init(deferred_init);
module_exit(deferred_exit);
