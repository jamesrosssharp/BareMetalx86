
all:	loader.bin

OBJ = main.o loaderui.o
HEADERS = loaderui.h

loader.bin:	boot.o $(OBJ) ../../OS/libkern.a
	ld --script link.ld boot.o $(OBJ) -o loader.bin -L../../OS -lkern

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -Werror -I../../OS -c $< -o $@

boot.o: boot.asm
	nasm -f elf boot.asm -o boot.o

.PHONY:	../../OS/libkern.a 

../../OS/libkern.a:
	make -C ../../OS

clean:
	rm loader.bin main.o boot.o
