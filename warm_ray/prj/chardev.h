#ifndef CHARDEV_H
#define CHARDEV_H

#include <linux/ioctl.h>

#define TASK_NAME_LENGTH 16
struct task_info {
    char task_name[TASK_NAME_LENGTH];
    int pid;
};

#define MAJOR_NUM 100
#define IOCTL_GET_TASK_INFO _IOR(MAJOR_NUM, 1, struct task_info* )
#define IOCTL_GET_TASK_COUNT _IOR(MAJOR_NUM, 2, int* )

#define DEVICE_FILE_NAME "char_dev"


#endif
