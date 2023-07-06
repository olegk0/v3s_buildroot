#!/bin/bash

fld=$1
pack=$2
fname=$3

if [ -z "$fname" ]
then
      fname=$pack
fi

make $pack-rebuild

if [ "$pack" = "linux" ]; then
sshpass -p 123 rsync output/images/zImage  root@192.168.0.166:/boot/
sshpass -p 123 rsync output/images/sun8i-v3s-licheepi-zero-dock.dtb  root@192.168.0.166:/boot/
else
arm-linux-gnueabihf-strip output/target/usr/$fld/$fname
sshpass -p 123 rsync output/target/usr/$fld/$fname root@192.168.0.166:/usr/$fld/
fi
