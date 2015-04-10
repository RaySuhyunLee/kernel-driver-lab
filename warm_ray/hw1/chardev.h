#ifndef CHARDEV_H
#define CHARDEV_H

#include <linux/ioctl.h>

struct task_info {
    char* task_name;
    int pid;
};

#define MAJOR_NUM 100
#define IOCTL_GET_TASK_INFO _IOR(MAJOR_NUM, 1, struct task_info* )
#define IOCTL_GET_TASK_COUNT _IOR(MAJOR_NUM, 2, int* count)

#define DEVICE_FILE_NAME "char_dev"


#endif
