#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h>   /* Needed for the macros */
#include <linux/fs.h>	/* Needed for I/O op */
#include <linux/miscdevice.h>
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/debugfs.h>

#define PATH "fortytwo" 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("This module is for debugfs!");

struct dentry *dir_entry, *file_entry;
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset);

static struct file_operations fops = {
	.read = dev_read,
	.write = dev_write,
};

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
   	return 1;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	return 1;
}

void	create_file(const char *name, umode_t mode)
{
	file_entry = debugfs_create_file(name, mode, dir_entry, NULL, &fops);
	if (file_entry == ERR_PTR(-ENODEV)) {
		printk(KERN_INFO "fail to create file, CONFIG_DEBUG_FS have to be set to yes. Change this value before retry\n");
	//	return 0;
	}
	else if (file_entry == NULL) {
		printk(KERN_INFO "Something wnet wront while create file! Abort!\n");
	//	return 0;
	}
	printk(KERN_INFO "[%s] file have been successfully created!\n", name);
	//return 1;
}

int	create_dir(void)
{
	dir_entry = debugfs_create_dir(PATH, NULL);
	if (dir_entry == ERR_PTR(-ENODEV)) {
		printk(KERN_INFO "fail to create dir, CONFIG_DEBUG_FS have to be set to yes. Change this value before retry\n");
		return 0;
	}
	else if (dir_entry == NULL) {
		printk(KERN_INFO "Something wnet wront while create PATH dir! Abort!\n");
		return 0;
	}	
	printk(KERN_INFO "PATH dir have been successfully created!\n");
	return 1;
}

static int __init debugfs_init(void)
{
	printk(KERN_INFO "init debugfs module!\n");
	if (create_dir() != 1) {
		printk(KERN_INFO "PATH dir failed to be created!\n");
		return 0;
	}
	create_file("id", 0666);
	create_file("jiffies", 0444);
	create_file("foo", 0644);
	return 0;
}

static void __exit debugfs_exit(void)
{
	printk(KERN_INFO "Cleaning up debugfs module.\n");
	debugfs_remove_recursive(dir_entry);
}

module_init(debugfs_init);
module_exit(debugfs_exit);
