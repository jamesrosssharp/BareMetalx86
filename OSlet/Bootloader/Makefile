all: MBR stage1_5 Loader

.PHONY: MBR stage1_5 Loader

MBR:
	make -C MBR

stage1_5:
	make -C stage1_5

Loader:
	make -C Loader

clean:
	make -C MBR clean
	make -C stage1_5 clean
	make -C Loader
