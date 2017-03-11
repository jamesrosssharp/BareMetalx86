all: run


secondstage.bin: secondstage.asm
	nasm -f bin -o secondstage.bin secondstage.asm

fd.img: bl.asm secondstage.bin payload.bin
	nasm -f bin -o fd.img bl.asm
	
fd.vmdk: fd.img
	@VBoxManage internalcommands createrawvmdk -filename fd.vmdk -rawdisk fd.img 

run:	fd.img
	@qemu-system-i386 -vga cirrus -drive file=fd.img,index=0,format=raw

bochs: fd.img
	@bochs 'ata0-master: type=disk, path="fd.img", cylinders=2, heads=16, spt=63' 'boot: disk'

clean:  
	rm fd.img secondstage.bin payload.bin main.o
