#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h>   /* Needed for the macros */
#include <linux/fs.h>	/* Needed for I/O op */
#include <linux/miscdevice.h>
#include <linux/uaccess.h>          // Required for the copy to user function

#define LOGIN "stmartin"
#define LOGIN_LEN 8
#define BUFF_SIZE 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("misc devies module");

static char   message[BUFF_SIZE] = {0};           ///< Memory for the string that is passed from userspace

static ssize_t	dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t	dev_write(struct file *, const char *, size_t, loff_t *);
static int	check_login(void);

static struct file_operations fops = {
	.read = dev_read,
 	.write = dev_write,
};

static struct miscdevice my_dev = {
	.minor = MISC_DYNAMIC_MINOR,
        .name = "fortytwo",
        .fops = &fops
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

static int __init misc_init(void)
{
    int retval;
    
    retval = misc_register(&my_dev);
    if (retval) 
	return retval;
    printk(KERN_INFO "fortytwo: got minor %i\n",my_dev.minor);
    return 0;
}

static void __exit misc_exit(void)
{
     printk(KERN_INFO "sorti du module, misc_exit\n");
     misc_deregister(&my_dev);
}

module_init(misc_init);
module_exit(misc_exit);
