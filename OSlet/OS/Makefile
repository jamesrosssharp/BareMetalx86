
all:	kernel.bin

HEADERS= kerndefs.h kernerr.h

kernel.bin:	Boot/boot.o main.o  libkern.a
	ld --script link.ld Boot/boot.o  main.o -o kernel.bin -L. -lkern

main.o:	main.c $(HEADERS)
	gcc -m32 -fno-stack-protector -Werror -c main.c -o main.o

LIBS =  Boot/boot.o Console/libconsole.a BIOS/libbios.a IO/libio.a	\
	Gfx/libgfx.a Mem/libmem.a Lib/libklib.a Devices/libdevices.a	\
	Filesystems/libfs.a

libkern.a:	$(LIBS)
	@printf "CREATE libkern.a\n \
    		ADDLIB Console/libconsole.a\n	\
    		ADDLIB BIOS/libbios.a\n	\
    		ADDLIB IO/libio.a\n	\
		ADDLIB Gfx/libgfx.a\n	\
		ADDLIB Mem/libmem.a\n	\
		ADDLIB Lib/libklib.a\n	\
		ADDLIB Devices/libdevices.a\n	\
		ADDLIB Filesystems/libfs.a\n	\
		SAVE\n		\
    		END" | ar -M
	ranlib libkern.a


.PHONY:	$(LIBS)

Boot/boot.o:	
	make -C Boot
	
Console/libconsole.a:
	make -C Console

BIOS/libbios.a:
	make -C BIOS

IO/libio.a:
	make -C IO

Gfx/libgfx.a:
	make -C Gfx

Mem/libmem.a:
	make -C Mem

Lib/libklib.a:
	make -C Lib

Devices/libdevices.a:
	make -C Devices

Filesystems/libfs.a:
	make -C Filesystems

clean:
	rm *.a *.o
	make -C BIOS clean
	make -C IO clean
	make -C Console clean
	make -C Boot	clean
	make -C Gfx 	clean
	make -C Mem	clean
	make -C Lib	clean
	make -C Devices clean
