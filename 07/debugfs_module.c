#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h>   /* Needed for the macros */
#include <linux/fs.h>	/* Needed for I/O op */
#include <linux/miscdevice.h>
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/debugfs.h>

#define PATH "fortytwo" 
#define LOGIN "stmartin"
#define LOGIN_LEN 8
#define BUFF_SIZE 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("This module is for debugfs!");

static char   message[BUFF_SIZE] = {0};           ///< Memory for the string that is passed from userspace

struct dentry *dir_entry, *file_entry;
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset);
static int	check_login(void);

static struct file_operations fops = {
	.read = dev_read,
	.write = dev_write,
};

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	if (*offset == 0 && len > LOGIN_LEN) {
		if (copy_to_user(buffer, LOGIN, LOGIN_LEN) == 0 )
			return (*offset = LOGIN_LEN);
		else {
			printk(KERN_INFO "Failed to send characters to the user\n");
			return -EFAULT;
		}
	}
   	return 0;
}

static int	check_login()
{
	if (strlen(message) == LOGIN_LEN && !(strcmp(LOGIN, message))) {
		printk(KERN_INFO "VALID value\n");
		return LOGIN_LEN;
	}
	printk(KERN_INFO "INVALID value\n");
   	return -EINVAL;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	if (len > BUFF_SIZE)
   		return 0;
	memset(message, 0, BUFF_SIZE);
	if (copy_from_user(message, buffer, len)) {
		printk(KERN_INFO "failed to copy message from user\n");
   		return -EFAULT;
	}
	return (check_login());
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
