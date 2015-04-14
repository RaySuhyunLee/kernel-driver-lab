obj-m += chardev.o
obj-m += msrdrv.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o caller1 caller1.c
	gcc -o caller2 caller2.c
	sudo insmod chardev.ko
	sudo insmod msrdrv.ko
	sudo mknod char_dev c 100 0
	sudo mknod /dev/msrdrv c 223 0

run1:
	./caller1

run2:
	sudo ./caller2
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm caller1
	rm caller2
	rm -f char_dev
	sudo rm -f /dev/msrdrv
	sudo rmmod chardev
	sudo rmmod msrdrv

submission:
	tar cvf 2012-11253_HyunilShin_2013-11419_SuhyunLee.tar ./
