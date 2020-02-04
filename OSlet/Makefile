all: run

.PHONY: payload.bin bootloader support

payload.bin:
	make -C OS

bootloader:
	make -C Bootloader

support:
	make -C support

hd.img: payload.bin bootloader support
	./scripts/makehdimage
	
hd.vmdk: hd.img
	@VBoxManage internalcommands createrawvmdk -filename hd.vmdk -rawdisk hd.img 

run:	hd.img
	@qemu-system-i386 -m 256 -vga std -drive file=hd.img,index=0,format=raw

bochs: hd.img
	#@bochs 'ata0-master: type=disk, path="hd.img", cylinders=2, heads=16, spt=63' 'boot: disk' 'pci: enabled=1, slot1=pcivga, chipset=i440fx'
	@bochs 'ata0-master: type=disk, path="hd.img"' 'boot: disk' 'pci: enabled=1, slot1=pcivga, chipset=i440fx' 'memory: guest=256, host=256'



clean:  
	rm hd.img hd.vmdk
	make -C OS clean
	make -C Bootloader clean
