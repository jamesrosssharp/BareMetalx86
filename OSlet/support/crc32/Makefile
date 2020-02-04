all: crc32

CRC=../../OS/Lib/crc32.c
HDR=../../OS/Lib/crc32.h

crc32: main.c $(CRC) $(HDR)
	gcc main.c $(CRC) -o crc32
