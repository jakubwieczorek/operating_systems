#!/bin/bash
# -o -- options, loop - pseudodevice like .iso or .img mount choose /dev/loop0, loop1 etc. which is free, offset tells where starts partition
MINIX_MOUNT='mount -t minix -o loop,offset=1483776 minix203.img mount_point'


$MINIX_MOUNT

