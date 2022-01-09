#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define PROCFS_MAX_SIZE		1024

#define procfs_dir_name		"list"
#define procfs_file_read	"preview"
#define procfs_file_write	"management"

struct proc_dir_entry *proc_list;
struct proc_dir_entry *proc_list_read;
struct proc_dir_entry *proc_list_write;

struct string_list {
	char* string;
	struct list_head list;
};

LIST_HEAD(my_list);

static int list_proc_show(struct seq_file *m, void *v)
{
    struct list_head *i, *tmp;
    struct string_list *entry;

    list_for_each_safe(i, tmp, &my_list) {
		entry = list_entry(i, struct string_list, list);
        seq_puts(m, entry->string);
        seq_puts(m, "\n");
	}

	return 0;
}

static int list_read_open(struct inode *inode, struct  file *file)
{
	return single_open(file, list_proc_show, NULL);
}

static int list_write_open(struct inode *inode, struct  file *file)
{
	return single_open(file, list_proc_show, NULL);
}

static int add_string(char* string, int loc) // loc 0 : addf , loc 1 : adde
{
	struct string_list *sle = kmalloc(sizeof *sle, GFP_KERNEL);

	if (!sle)
		return -ENOMEM;

	sle->string = string;

	if (loc) list_add(&sle->list, &my_list);
    else list_add_tail(&sle->list, &my_list);

	return 0;
}

static int del_string(char* string, int opt) // opt 0 : delf , opt 1 : dela
{
	struct list_head *i, *tmp;
	struct string_list *sle;

	list_for_each_safe(i, tmp, &my_list) {
		sle = list_entry(i, struct string_list, list);
		if (!strcmp(sle->string, string)) {
			list_del(i);
            kfree(sle->string);
			kfree(sle);
			if(!opt) return 0;
		}
	}

	if(!opt) return -ENOMEM;
	return 0;
}

static void destroy_list(void)
{
	struct list_head *i, *n;
	struct string_list *sle;

	list_for_each_safe(i, n, &my_list) {
		sle = list_entry(i, struct string_list, list);
		list_del(i);
        kfree(sle->string);
		kfree(sle);
	}
}

static ssize_t list_write(struct file *file, const char __user *buffer,
			  size_t count, loff_t *offs)
{
	char local_buffer[PROCFS_MAX_SIZE], temp_local_buffer[PROCFS_MAX_SIZE];
    char *split_local_buffer[2];
    char *temp, *ptr = NULL; 
	unsigned long local_buffer_size = 0;
    int idx = 0;
    char* string;

	local_buffer_size = count;
	if (local_buffer_size > PROCFS_MAX_SIZE)
		local_buffer_size = PROCFS_MAX_SIZE;

	memset(local_buffer, 0, PROCFS_MAX_SIZE);
	if (copy_from_user(local_buffer, buffer, local_buffer_size))
		return -EFAULT;
	
    // parse the command
    strcpy(temp_local_buffer, local_buffer);
    temp = temp_local_buffer; 
    
    while((ptr = strsep(&temp, " ")) != NULL) { 
        split_local_buffer[idx] = ptr;
        if(!idx)  split_local_buffer[idx][strlen(split_local_buffer[idx])] = '\0';
        else split_local_buffer[idx][strlen(split_local_buffer[idx]) - 1] = '\0';
        idx++;
    }

    // add/delete elements.
    if(!strcmp(split_local_buffer[0], "addf"))
    {
        string = kmalloc(sizeof(char) * strlen(split_local_buffer[1]), GFP_KERNEL);
        strcpy(string, split_local_buffer[1]);
        add_string(string, 1);
    }
    else if(!strcmp(split_local_buffer[0], "adde"))
    {
        string = kmalloc(sizeof(char) * strlen(split_local_buffer[1]), GFP_KERNEL);
        strcpy(string, split_local_buffer[1]);
        add_string(string, 0);
    }
    else if(!strcmp(split_local_buffer[0], "delf"))
    {
		del_string(split_local_buffer[1], 0);
    }
    else if(!strcmp(split_local_buffer[0], "dela"))
    {
        del_string(split_local_buffer[1], 1);
    }

	return local_buffer_size;
}

static const struct proc_ops r_pos = {
	//.owner		= THIS_MODULE,
	.proc_open		= list_read_open,
	.proc_read		= seq_read,
	.proc_release		= single_release,
};

static const struct proc_ops w_pos = {
	//.owner		= THIS_MODULE,
	.proc_open		= list_write_open,
	.proc_write		= list_write,
	.proc_release		= single_release,
};

static int list_init(void)
{
	proc_list = proc_mkdir(procfs_dir_name, NULL);
	if (!proc_list)
		return -ENOMEM;

	proc_list_read = proc_create(procfs_file_read, 0000, proc_list,
				     &r_pos);
	if (!proc_list_read)
		goto proc_list_cleanup;

	proc_list_write = proc_create(procfs_file_write, 0000, proc_list,
				      &w_pos);
	if (!proc_list_write)
		goto proc_list_read_cleanup;

	return 0;

proc_list_read_cleanup:
	proc_remove(proc_list_read);
proc_list_cleanup:
	proc_remove(proc_list);
	return -ENOMEM;
}

static void list_exit(void)
{
    destroy_list();
	proc_remove(proc_list);
}

module_init(list_init);
module_exit(list_exit);

MODULE_DESCRIPTION("Linux kernel list API");
MODULE_AUTHOR("Yurim Jang");
MODULE_LICENSE("GPL v2");
