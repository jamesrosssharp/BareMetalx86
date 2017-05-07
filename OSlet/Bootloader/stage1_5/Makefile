all: stage1_5.bin

stage1_5.bin: stage1_5.asm FATdefs.h
	nasm -f bin -o stage1_5.bin stage1_5.asm

clean:
	rm stage1_5.bin
