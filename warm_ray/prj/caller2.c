#include "msrdrv.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

static int loadDriver() {
	int fd;
	fd = open("/dev/" DEV_NAME, O_RDWR);
	if (fd == -1) {
		perror("Failed to open /dev/" DEV_NAME);
	}
	return fd;
}

static void closeDriver(int fd) {
	int e;
	e = close(fd);
	if (e == -1)
		perror("Failed to close fd");
}

static void resetPMU(int fd) {
	static struct MsrInOut msr_reset[] = {
		{ MSR_WRITE, 0xc1, 0x00, 0x00 },
		{ MSR_WRITE, 0Xc2, 0X00, 0X00 },
		{ MSR_WRITE, 0Xc3, 0X00, 0X00 },
		{ MSR_WRITE, 0Xc4, 0x00, 0x00 },
		{ MSR_WRITE, 0x309, 0x00, 0x00 },
		{ MSR_WRITE, 0x30a, 0x00, 0x00 },
		{ MSR_WRITE, 0x30b, 0x00, 0x00 },
		{ MSR_STOP, 0x00, 0x00 }
	};
	printf("###### Reset PMU Counter ######\n");
	ioctl(fd, IOCTL_MSR_CMDS, (long long)msr_reset);
}


/* 
 * PMU Event Handler
 */
static int PMU_EVENT;

enum PMU_EVENT_SET {
	EVENT_USER   = 1,
	EVENT_KERNEL = 2,
};


static void selectEvent(int fd, int event) {
	struct MsrInOut* msr_select;

	static struct MsrInOut msr_select_user[] = {
		{ MSR_WRITE, 0x186, 0x004101c2, 0x00 },
		{ MSR_WRITE, 0x187, 0x0041010e, 0x00 },
		{ MSR_WRITE, 0x188, 0x01c1010e, 0x00 },
		{ MSR_WRITE, 0x189, 0x004101a2, 0x00 },
		{ MSR_STOP, 0x00, 0x00 }
	};
	static struct MsrInOut msr_select_kernel[] = {
		{ MSR_WRITE, 0x186, 0x004201c2, 0x00 },
		{ MSR_WRITE, 0x187, 0x0042010e, 0x00 },
		{ MSR_WRITE, 0x188, 0x01c2010e, 0x00 },
		{ MSR_WRITE, 0x189, 0x004201a2, 0x00 },
		{ MSR_STOP, 0x00, 0x00 }
	};

	PMU_EVENT = event;
	switch(event) {
		case EVENT_USER:
			msr_select = msr_select_user;
			break;
		case EVENT_KERNEL:
			msr_select = msr_select_kernel;
			break;
	}
	ioctl(fd, IOCTL_MSR_CMDS, (long long)msr_select);
	printf("###### Event Selected ######\n");
}

static void readPMU(int fd) {
	struct MsrInOut msr_read[] = {
		{ MSR_READ, 0xc1, 0x00 },
		{ MSR_READ, 0xc2, 0x00 },
		{ MSR_READ, 0xc3, 0x00 },
		{ MSR_READ, 0xc4, 0x00 },
		{ MSR_READ, 0x309, 0x00 },
		{ MSR_READ, 0x30a, 0x00 },
		{ MSR_READ, 0x30b, 0x00 },
		{ MSR_RDTSC, 0x00, 0x00 },
		{ MSR_STOP, 0x00, 0x00 }
	};
	ioctl(fd, IOCTL_MSR_CMDS, (long long)msr_read);
	
	printf("--------------------------------\n");
	switch(PMU_EVENT) {
		case EVENT_USER:
			printf("target: user space\n");
			printf("uops retired:       %7lld\n", msr_read[0].value);
			printf("uops issued:        %7lld\n", msr_read[1].value);
			printf("stalled cycles:     %7lld\n", msr_read[2].value);
			printf("resource stalls:    %7lld\n", msr_read[3].value);
			break;
		case EVENT_KERNEL:
			printf("target: kernel space\n");
			printf("uops retired:       %7lld\n", msr_read[0].value);
			printf("uops issued:        %7lld\n", msr_read[1].value);
			printf("stalled cycles:     %7lld\n", msr_read[2].value);
			printf("resource stalls:    %7lld\n", msr_read[3].value);
			break;
	}
	printf("--------------------------------\n");
	printf("instr retired:      %7lld\n", msr_read[4].value);
	printf("core cycles:        %7lld\n", msr_read[5].value);
	printf("ref cycles:         %7lld\n", msr_read[6].value);
	printf("--------------------------------\n");
	printf("Time Stamp Counter: %7lld\n\n", msr_read[7].value);
}

static void startPMU(int fd) {
	static struct MsrInOut msr_start[] = {
		{ MSR_WRITE, 0x38f, 0x0f, 0x07 },
		{ MSR_STOP, 0x00, 0x00 }
	};
	printf("###### Start PMU Counter ######\n");
	ioctl(fd, IOCTL_MSR_CMDS, (long long)msr_start);
}

static void stopPMU(int fd) {
	static struct MsrInOut msr_stop[] = {
		{ MSR_WRITE, 0x38f, 0x00, 0x00 },
		{ MSR_STOP, 0x00, 0x00 }
	};
	printf("###### Stop PMU Counter ######\n");
	ioctl(fd, IOCTL_MSR_CMDS, (long long)msr_stop);
}

static void enableFFC(int fd) {
	static struct MsrInOut msr_enable_ffc[] = {
		{ MSR_WRITE, 0x38d, 0x222, 0x00 },
		{ MSR_STOP, 0x00, 0x00 }
	};
	ioctl(fd, IOCTL_MSR_CMDS, (long long)msr_enable_ffc);
	printf("###### FFCs Enabled ######\n");
}

static void disableFFC(int fd) {
	static struct MsrInOut msr_disable_ffc[] = {
		{ MSR_WRITE, 0x38d, 0x00, 0x00 },
		{ MSR_STOP, 0x00, 0x00 }
	};
	ioctl(fd, IOCTL_MSR_CMDS, (long long)msr_disable_ffc);
	printf("###### FFCs Disabled ######\n");
}

int main(void) {
	int fd;
	int cmd;
	int read_cnt;

	fd = loadDriver();

	selectEvent(fd, EVENT_USER);

	printf("Hello, User!\n");
	printf("This is Super Easy-to-Use PMU Monitor\n");
	printf("implemented for system programming assignment :)\n\n");

	printf(" --------- Usage ----------\n");
	printf("| 0: Exit                 |\n");
	printf("| 1: Reset PMU Counter    |\n");
	printf("| 2: Start PMU Counter    |\n");
	printf("| 3: Stop PMU Counter     |\n");
	printf("| 4: Read PMU Counter     |\n");
	printf("| 5: Enable FFC           |\n");
	printf("| 6: Disable FFC          |\n");
	printf("| 7: Set Event TO MONITOR |\n");
	printf(" --------------------------\n\n");

	while(1) {
		printf("Select Instruction > ");
		read_cnt = scanf("%d", &cmd);
		if (read_cnt != 1)
			break;

		if (cmd == 0)
			break;

		switch(cmd) {
			case 1:
				resetPMU(fd);
				break;
			case 2:
				startPMU(fd);
				break;
			case 3:
				stopPMU(fd);
				break;
			case 4:
				readPMU(fd);
				break;
			case 5:
				enableFFC(fd);
				break;
			case 6:
				disableFFC(fd);
				break;
			case 7:
				printf(" 1: EVENT_USER\n");
				printf(" 2: EVENT_KERNEL\n");
				while(1) {
					printf("choose > ");
					read_cnt = scanf("%d", &cmd);
					if (read_cnt == 1 && cmd > 0 && cmd < 3)
						break;
					printf("Invalid command!\n");
				}
				selectEvent(fd, cmd);
				break;
			default:
				printf("Invalid command!\n");
		}
	}

	closeDriver(fd);
	return 0;
}
