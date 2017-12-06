#!/bin/bash
# -o -- options, loop - pseudodevice like .iso or .img mount choose /dev/loop0, loop1 etc. which is free, offset tells where starts partition

MOUNT_POINT_NAME=minix_mount_point
MINIX=$1

if test ! -e $MOUNT_POINT_NAME; then
	mkdir $MOUNT_POINT_NAME
fi

mount -t minix -o loop,offset=1483776 $MINIX $MOUNT_POINT_NAME

