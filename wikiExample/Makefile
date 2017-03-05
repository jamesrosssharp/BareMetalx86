all: run

fd.bin: bl.asm
	nasm -f bin -o fd.bin bl.asm

run:	fd.bin
	qemu-system-i386 -fda fd.bin
