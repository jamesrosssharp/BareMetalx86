#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../OS/Lib/crc32.h"

int main(int argc, char** argv)
{

	if (argc != 2)
	{
		printf("Usage: crc32 <file>\n");
		exit(1);
	}

	FILE* f = fopen(argv[1], "rw");

	fseek(f, 0L, SEEK_END);
	uint32_t sz = ftell(f);

	char* buf = malloc(sz);

	rewind(f);

	if (buf == NULL)
	{
		printf("Couldn't malloc buffer\n");
		exit(2);
	}

	if (fread(buf, 1, sz, f) != sz)
	{
		printf("Couldn't read from file!\n");
		exit(3);
	}

	lib_crc32_init(CRC32);

	unsigned char* data = buf;
	unsigned int crc = 0xffffffff;

	for (int i = 0; i < sz; i++)
	{
		crc = lib_crc32_compute(*data++, crc);
	}

	crc = lib_crc32_finalise(crc);

	printf("CRC32: %x\n", crc);

	free(buf);

	fclose(f);
}
