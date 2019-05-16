#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>

static struct proc_dir_entry *proc_entry;

static ssize_t mywrite(struct file *file, const char *buffer,size_t len, loff_t *ppos)
{
	printk(KERN_INFO "in mywrite\n");
	return 1;
}

static ssize_t myread(struct file *file, char *buffer,size_t len, loff_t *ppos) 
{
	printk(KERN_INFO "in myread\n");
	return 1;
}

static struct file_operations myops =
{
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};

static int 	__init mounts_init(void)
{
	printk(KERN_INFO "mounts: mounts module init!\n");
	proc_entry = proc_create("mymounts", 0440, NULL, &myops);
	if (proc_entry == NULL)
   		return -ENOMEM;
	return 0;
}

static void	__exit mounts_exit(void)
{
	proc_remove(proc_entry);
	printk(KERN_INFO "mounts: mounts module exit!\n");
}

module_init(mounts_init);
module_exit(mounts_exit);
