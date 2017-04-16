#!/bin/sh

# create image file
dd if=/dev/zero of=fat.img bs=1M count=10

# loop mount
losetup /dev/loop0 fat.img
mkfs.vfat -F 16 /dev/loop0

mkdir /tmp/fat
mount /dev/loop0 /tmp/fat


mkdir /tmp/fat/MYDIR
chmod 777 /tmp/fat/MYDIR

echo "Hello world!" > /tmp/fat/MYDIR/a.txt

mkdir /tmp/fat/MYDIR/SUBDIR
echo "Wassup doc!" > /tmp/fat/MYDIR/SUBDIR/b.txt

umount /dev/loop0

rmdir /tmp/fat

# remove loop device

losetup -D
