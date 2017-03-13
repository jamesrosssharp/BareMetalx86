all: run

.PHONY: payload.bin

payload.bin:
	make -C OS
	cp OS/kernel.bin payload.bin

stage1_5.bin: Bootloader/stage1_5.asm
	cd Bootloader && nasm -f bin -o ../stage1_5.bin stage1_5.asm

hd.img: Bootloader/bl.asm stage1_5.bin payload.bin
	cd Bootloader && nasm -f bin -o ../hd.img bl.asm
	
hd.vmdk: hd.img
	@VBoxManage internalcommands createrawvmdk -filename hd.vmdk -rawdisk hd.img 

run:	hd.img
	@qemu-system-i386 -vga cirrus -drive file=hd.img,index=0,format=raw

bochs: hd.img
	@bochs 'ata0-master: type=disk, path="hd.img", cylinders=2, heads=16, spt=63' 'boot: disk'

clean:  
	rm hd.img stage1_5.bin payload.bin main.o
	make -C OS clean
