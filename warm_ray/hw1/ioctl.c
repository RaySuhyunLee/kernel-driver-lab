#include "chardev.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>			/* open, close */
#include <unistd.h>			/* exit */
#include <sys/ioctl.h>	    /* ioctl */

void ioctl_get_task_count(int file_desc, int* count) {
	int ret_val;
	ret_val = ioctl(file_desc, IOCTL_GET_TASK_COUNT, count);

	if (ret_val < 0) {
		printf("ioctl_get_task_count failed: %d\n", ret_val);
		exit(-1);
	}
}

void ioctl_get_task_info(int file_desc, struct task_info * tinfo) {
	int ret_val;

	ret_val = ioctl(file_desc, IOCTL_GET_TASK_INFO, tinfo);

	if (ret_val<0) {
		printf("ioctl_get_task_info failed:%d\n", ret_val);
		exit(-1);
	}
}

void get_all_task(int file_desc, int count) {
	struct task_info tinfo;
	ioctl_get_task_info(file_desc, &tinfo);
	if (count > 1)
		get_all_task(file_desc, count-1);
	for (; count>1; count--)
		printf(" ");
	printf("\\- %s(%d)\n", tinfo.task_name, tinfo.pid);
}

int main() {
	int file_desc, i, j;
	int count;

	file_desc = open(DEVICE_FILE_NAME, 0);
	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}
	
	ioctl_get_task_count(file_desc, &count);
	printf("count: %d\n", count);

	get_all_task(file_desc, count);

	close(file_desc);

	return 0;
}

