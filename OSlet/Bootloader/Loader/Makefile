
all:	loader.bin


loader.bin:	boot.o main.o  ../../OS/libkern.a
	ld --script link.ld boot.o main.o -o loader.bin -L../../OS -lkern

main.o:	main.c
	gcc -m32 -fno-stack-protector -c main.c -o main.o -I../../OS

boot.o: boot.asm
	nasm -f elf boot.asm -o boot.o

.PHONY:	../../OS/libkern.a 

../../OS/libkern.a:
	make -C ../../OS

