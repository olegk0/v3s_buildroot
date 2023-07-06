#!/bin/bash


sudo umount /dev/sdc1
sudo umount /dev/sdc2

sudo dd if=output/images/u-boot-sunxi-with-spl.bin of=/dev/sdc bs=1024 seek=8

sync
