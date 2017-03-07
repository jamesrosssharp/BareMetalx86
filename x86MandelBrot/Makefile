all: run

main.o:	main.c
	gcc -m32 -fno-stack-protector -c main.c -o main.o

payload.bin: main.o
	ld --script link.ld main.o -o payload.bin


secondstage.bin: secondstage.asm
	nasm -f bin -o secondstage.bin secondstage.asm

fd.img: bl.asm secondstage.bin payload.bin
	nasm -f bin -o fd.img bl.asm
	

run:	fd.img
	@qemu-system-i386 -vga cirrus -drive file=fd.img,index=0,if=floppy,format=raw

clean:  
	rm fd.img secondstage.bin payload.bin main.o
