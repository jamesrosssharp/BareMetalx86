
all:	loader.bin

OBJ = main.o loaderui.o
HEADERS = loaderui.h

loader.bin:	boot.o bootsplash.o $(OBJ) link.ld ../../OS/libkern.a
	ld --script link.ld boot.o bootsplash.o $(OBJ) -o loader.bin -L../../OS -lkern
	@if [ `stat -c%s loader.bin` -gt 262144 ]; then echo "!!!!"; echo "Loader too big!"; echo "!!!!"; exit 1; fi
	ld --script link-elf.ld boot.o bootsplash.o $(OBJ) -o loader.elf -L../../OS -lkern


%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -Werror -I../../OS \
		-I../../assets/LoaderScreen -c $< -o $@

boot.o: boot.asm
	nasm -f elf boot.asm -o boot.o

.PHONY:	../../assets/LoaderScreen/bootsplash.c

../../assets/LoaderScreen/bootsplash.c:
	make -C ../../assets/LoaderScreen

bootsplash.o: ../../assets/LoaderScreen/bootsplash.o
	gcc -m32 -fno-stack-protector -Werror -c ../../assets/LoaderScreen/bootsplash.c


../../OS/libkern.a:
	make -C ../../OS

clean:
	rm loader.bin main.o boot.o
