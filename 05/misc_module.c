#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h>   /* Needed for the macros */
#include <linux/fs.h>	/* Needed for I/O op */
#include <linux/miscdevice.h>
#include <linux/uaccess.h>          // Required for the copy to user function

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("misc devies module");

static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored

static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

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
	if (copy_to_user(buffer, message, size_of_message) == 0 ) {
		printk(KERN_INFO "Sent %d characters to the user\n", size_of_message);
      		return (size_of_message = 0);
   	}
   	printk(KERN_INFO "Failed to send characters to the user\n");
   	return -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
   sprintf(message, "%s", buffer);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "Received %zu characters from the user\n", len);
   return len;
}

static int __init misc_init(void)
{
    int retval = misc_register(&my_dev);
    if (retval) 
	return retval;
    printk("fortytwo: got minor %i\n",my_dev.minor);
    printk(KERN_INFO "entre par misc init\n");
    return 0;
}

static void __exit misc_exit(void)
{
     printk(KERN_INFO "sort par misc exit\n");
     misc_deregister(&my_dev);
}

module_init(misc_init);
module_exit(misc_exit);
