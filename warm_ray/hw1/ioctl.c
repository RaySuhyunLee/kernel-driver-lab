#include "chardev.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>			/* open, close */
#include <unistd.h>			/* exit */
#include <sys/ioctl.h>	    /* ioctl */

void ioctl_get_task(int file_desc, struct task_info * tinfo) {
	int ret_val;

    ret_val = ioctl(file_desc, IOCTL_GET_TASK_INFO, tinfo);

		printf("pid: %d", tinfo->pid);

	if (ret_val<0) {
		printf("ioctl_get_tlist failed:%d\n", ret_val);
		exit(-1);
	}
}

main() {
	int file_desc, ret_val;
    struct task_info task;

	file_desc = open(DEVICE_FILE_NAME, 0);
	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}

    ioctl_get_task(file_desc, &task);
	close(file_desc);

}
