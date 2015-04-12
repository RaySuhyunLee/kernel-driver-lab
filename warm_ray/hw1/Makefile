obj-m += chardev.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o caller caller.c
	sudo mknod char_dev c 100 0
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm caller
	rm -f char_dev
