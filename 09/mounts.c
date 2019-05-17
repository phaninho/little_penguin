#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include "../fs/mount.h"
#include <linux/fs_struct.h>

MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("proc mounted list module");
MODULE_LICENSE("GPL");

#define PROC_ENTRY_FILENAME "mymounts"
#define BUFF_SIZE 256

static struct proc_dir_entry	*proc_entry;
static struct dentry		*curdentry;
//static char			*message_begin_address;
static char   			message[BUFF_SIZE] = {0};

static void	append_message(void)
{
	char	path_buff[256] = {0};
	
	printk(KERN_INFO "mounts: inside append_message [%s]\n", curdentry->d_name.name);
	strcat(message, curdentry->d_name.name);
	strcat(message, "\t");
	strcat(message, dentry_path_raw(curdentry, path_buff, 256));
	strcat(message, "\n");
}

static ssize_t myread(struct file *file, char *buffer,size_t len, loff_t *ppos) 
{
	int	size_of_message;

	if (message[0] == 0) {
	/*	message_begin_address = message;
		printk(KERN_INFO "on est laaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
	}
	else
	{
		memset(message, 0, BUFF_SIZE);
		message = message_begin_address;
	}*/
		printk(KERN_INFO "mounts: in myread\n");
		list_for_each_entry(curdentry, &current->fs->root.mnt->mnt_root->d_subdirs, d_child)
		{
			printk(KERN_INFO "mounts: in list_for_each\n");
			if (curdentry->d_flags & DCACHE_MOUNTED)
				append_message();
		}
	}
	size_of_message = strlen(message);
	printk(KERN_INFO "size_of_message [%d]\n", size_of_message);
	if (*ppos == 0 && len > 5) {
		if (copy_to_user(buffer, message, size_of_message) != 0) {
			printk(KERN_INFO "copy to user failed\n");
			return -1;
		}
		*ppos = size_of_message;
		return (size_of_message);
	}
	return 0;
}

static struct file_operations myops =
{
	.owner = THIS_MODULE,
	.read = myread,
};

static int 	__init mounts_init(void)
{
	printk(KERN_INFO "mounts: mounts module init!\n");
	proc_entry = proc_create(PROC_ENTRY_FILENAME, 0440, NULL, &myops);
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
