all: mbr.bin

mbr.bin: bl.asm
	nasm -f bin -o mbr.bin bl.asm

clean:
	rm mbr.bin
