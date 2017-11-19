#!/bin/bash
# -o -- options, loop - pseudodevice like .iso or .img mount choose /dev/loop0, loop1 etc. which is free, offset tells where starts partition

MOUNT_POINT_NAME=mount_point
MINIX=minix203.img

if test ! -e mount_point; then
	mkdir mount_point
fi

mount -t minix -o loop,offset=1483776 $MINIX $MOUNT_POINT_NAME

