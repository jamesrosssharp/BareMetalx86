all: boot.o

boot.o:	boot.asm
	nasm -f elf boot.asm -o boot.o

clean:
	rm boot.o 
