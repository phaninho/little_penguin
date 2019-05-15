#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h>   /* Needed for the macros */
#include <linux/fs.h>	/* Needed for I/O op */
#include <linux/miscdevice.h>
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/debugfs.h>
#include <linux/timer.h>

#define PATH "fortytwo" 
#define LOGIN "stmartin"
#define LOGIN_LEN 8
#define BUFF_SIZE 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("This module is for debugfs!");

static char   message[BUFF_SIZE] = {0};
static char   foo_message[BUFF_SIZE] = {0};
static struct mutex foo_mutex;
static short  size_of_message;

struct dentry *dir_entry, *file_entry;
static ssize_t id_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);
static ssize_t id_read(struct file *filep, char *buffer, size_t len, loff_t *offset);
static ssize_t foo_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);
static ssize_t foo_read(struct file *filep, char *buffer, size_t len, loff_t *offset);
static int	check_login(void);

static struct file_operations id_ops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write,
};

static struct file_operations foo_ops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write,
};

static ssize_t foo_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int mutex_state;
	int error_count;
	int read_offset;
       
	error_count = 0;
	read_offset = size_of_message - *offset;
	if ((mutex_state = mutex_lock_interruptible(&foo_mutex))) {
		printk(KERN_INFO "debufs_module: error, in foo_read mutex_state [%d]\n", mutex_state);
		return mutex_state;
	}
	error_count = copy_to_user(buffer, (foo_message + *offset), read_offset);
	if (error_count == 0) {
		*offset +=  read_offset - error_count;
		mutex_unlock(&foo_mutex);
      		return (read_offset - error_count);
   	}
	mutex_unlock(&foo_mutex);
    	return -EFAULT;
}

static ssize_t foo_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int mutex_state;

	if (*offset + len > BUFF_SIZE)
		return (-EINVAL);
	if ((mutex_state = mutex_lock_interruptible(&foo_mutex))) {
		printk(KERN_INFO "debufs_module: error, in foo_write mutex_state [%d]\n", mutex_state);
		return mutex_state;
	}
	if (copy_from_user(foo_message, buffer, len)) {
		mutex_unlock(&foo_mutex);
		printk(KERN_INFO "debugfs_module: copy_from_user failed\n");
		return -EINVAL;
	}
	size_of_message = len;
	*offset += len;
	mutex_unlock(&foo_mutex);
	return len;
}

static ssize_t id_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
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

static ssize_t id_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	if (len > BUFF_SIZE)
   		return 0;
	memset(message, 0, BUFF_SIZE);
	if (copy_from_user(message, buffer, len)) {
		printk(KERN_INFO "failed to copy message from user\n");
   		return -EFAULT;
	}
	printk(KERN_INFO "debugfs_module: offset [%lld] %ld\n", *offset, strlen(message));
	return (check_login());
}

void	create_file(const char *name, umode_t mode, struct file_operations *fops)
{
	if (fops == NULL)
		file_entry = debugfs_create_u64(name, mode, dir_entry, &jiffies_64);
	else
		file_entry = debugfs_create_file(name, mode, dir_entry, NULL, fops);
	if (file_entry == ERR_PTR(-ENODEV)) {
		printk(KERN_INFO "fail to create file, CONFIG_DEBUG_FS have to be set to yes. \
				Change this value before retry\n");
	}
	else if (file_entry == NULL) {
		printk(KERN_INFO "Something wnet wront while create file! Abort!\n");
	}
	printk(KERN_INFO "[%s] file have been successfully created!\n", name);
}

int	create_dir(void)
{
	dir_entry = debugfs_create_dir(PATH, NULL);
	if (dir_entry == ERR_PTR(-ENODEV)) {
		printk(KERN_INFO "fail to create dir, CONFIG_DEBUG_FS have to be set to yes. \
				Change this value before retry\n");
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
	mutex_init(&foo_mutex);
	create_file("id", 0666, &id_ops);
	create_file("jiffies", 0444, NULL);
	create_file("foo", 0644, &foo_ops);
	return 0;
}

static void __exit debugfs_exit(void)
{
	printk(KERN_INFO "Cleaning up debugfs module.\n");
	debugfs_remove_recursive(dir_entry);
}

module_init(debugfs_init);
module_exit(debugfs_exit);
