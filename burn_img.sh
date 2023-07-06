#!/bin/bash


sudo umount /dev/sdc1
sudo umount /dev/sdc2

sudo dd if=output/images/sdcard.img of=/dev/sdc bs=4M

sync

sudo parted /dev/sdc resizepart 2 100%

sync

sudo e2fsck -f /dev/sdc2

sync

sudo resize2fs /dev/sdc2

sync
