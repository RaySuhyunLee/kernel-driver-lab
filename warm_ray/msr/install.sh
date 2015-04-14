#!/bin/bash

if [ "$(whoami)" != "root" ] ; then
	echo -e "\n\tYou must be root to run this script.\n"
	exit 1
fi

insmod msrdrv.ko
mknod /dev/msrdrv c 223 0
