obj-m := msrdrv.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -g -I. -O2 -o ioctl ioctl.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm ioctl
