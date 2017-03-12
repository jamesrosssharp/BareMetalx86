
all:	kernel.bin

HEADERS= kerndefs.h kernerr.h

kernel.bin:	Boot/boot.o main.o  libkern.a
	ld --script link.ld Boot/boot.o  main.o -o kernel.bin -L. -lkern

main.o:	main.c $(HEADERS)
	gcc -m32 -fno-stack-protector -c main.c -o main.o

libkern.a:	Console/libconsole.a BIOS/libbios.a	IO/libio.a
	@printf "CREATE libkern.a\n \
    		ADDLIB Console/libconsole.a\n	\
    		ADDLIB BIOS/libbios.a\n	\
    		ADDLIB IO/libio.a\n	\
		SAVE\n		\
    		END" | ar -M
	ranlib libkern.a


.PHONY: Boot/boot.o Console/libconsole.a BIOS/libbios.a IO/libio.a

Boot/boot.o:	
	make -C Boot
	
Console/libconsole.a:
	make -C Console

BIOS/libbios.a:
	make -C BIOS

IO/libio.a:
	make -C IO

clean:
	rm *.a *.o
	make -C BIOS clean
	make -C IO clean
	make -C Console clean
	make -C Boot	clean