/*
 * chardev.c - Create an input/output character device
 */

#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/fs.h>
#include <asm/uaccess.h>	/* for get_user and put_user */
#include <linux/sched.h>
#include <linux/version.h>	/* to support ioctl which vary in version */

#include "chardev.h"
#define SUCCESS 0
#define DEVICE_NAME "char_dev"
#define BUF_LEN 80

#define DEBUG

static int Device_Open = 0;
static struct task_struct* Task = NULL;

static int device_open(struct inode *inode, struct file *file) {
#ifdef DEBUG
	printk(KERN_INFO "device_open(%p)\n", file);
#endif

	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file) {
#ifdef DEBUG
	printk(KERN_INFO "device_release(%p, %p)\n", inode, file);
#endif

	Device_Open--;

	module_put(THIS_MODULE);
	return SUCCESS;
}

/*
 * This function is called when sombody tries to
 * read the device file. It copies Message to buffer.
 */
static ssize_t device_read(struct file *file,
		char __user *buffer, size_t length, loff_t *offset) {
	int bytes_read = 0;

#ifdef DEBUG
	printk(KERN_INFO "device_read(%p, %p, %d)\n", file, buffer, length);
#endif

	return bytes_read;
}
/*
 * This function is called when somebody tries to
 * write into our device file.
 * It copies the data from user buffer and stores
 * it in Message.
 */
static ssize_t device_write(struct file *file,
		const char __user *buffer, size_t length, loff_t *offset) {
	int i=0;

#ifdef DEBUG
	printk(KERN_INFO "device_write(%p, %s, %d)\n", file, buffer, length);
#endif

	return i;
}


/*
 * Support different versions
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int device_ioctl(struct inode *inode,
		struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) {
#else
static long device_ioctl(struct file *file,
		unsigned int ioctl_num, unsigned long ioctl_param) {
#endif

	int i;
	struct task_struct *task;	// for IOCTL_GET_TASK_COUNT
	struct task_info tinfo;		// for IOCTL_GET_TASK_INFO

#ifdef DEBUG
	printk(KERN_INFO "device ioctl call with %u, %lu\n", ioctl_num, ioctl_param);
#endif

	switch (ioctl_num) {
		case IOCTL_GET_TASK_COUNT:
			Task = current;
			task = Task;
			i = 0;
			while(true) {
				i++;
				if (task->pid==0)
					break;

				task = task->parent;
			}
			put_user(i, (int*)ioctl_param);
			break;
		case IOCTL_GET_TASK_INFO:
			/*
			 * copy and modify value of task_info struct
			 */
			copy_from_user((void*)&tinfo, (void*)ioctl_param, sizeof(struct task_info));
			tinfo.pid = Task->pid;
			for(i=0; Task->comm[i]!='\0' && i<TASK_NAME_LENGTH; i++)
				tinfo.task_name[i] = Task->comm[i];
			tinfo.task_name[i] = '\0';

			copy_to_user((void*)ioctl_param, (void *)&tinfo, sizeof(struct task_info));

			Task = Task->parent;
	}

	return SUCCESS;
}

/* Module Declarations */

struct file_operations Fops = {
	.read = device_read,
	.write = device_write,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
    .ioctl = device_ioctl,
#else
	.unlocked_ioctl = device_ioctl,
#endif
	.open = device_open,
	.release = device_release
};

int init_module() {
	int ret_val;

	ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);

	if (ret_val<0) {
		printk(KERN_ALERT "%s failed with %d\n",
				"Sorry, registering the character device ", ret_val);
		return ret_val;
	}

	printk(KERN_INFO "%s\n The major device number is %d.\n",
			"Registeration is a success", MAJOR_NUM);
	printk(KERN_INFO "mknod %s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM);

	return 0;
}

void cleanup_module() {
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}
