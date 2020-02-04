/*

	Code from Dr Dobbs journal

					*/

#include "crc32.h"

unsigned int crcTable[256];

// http://www.hackersdelight.org/hdcodetxt/crc.c.txt

unsigned int reverse(unsigned int x) {
   x = ((x & 0x55555555) <<  1) | ((x >>  1) & 0x55555555);
   x = ((x & 0x33333333) <<  2) | ((x >>  2) & 0x33333333);
   x = ((x & 0x0F0F0F0F) <<  4) | ((x >>  4) & 0x0F0F0F0F);
   x = (x << 24) | ((x & 0xFF00) << 8) |
       ((x >> 8) & 0xFF00) | (x >> 24);
   return x;
}


void lib_crc32_init(enum CRC32Generator generator)
{
	unsigned int crc;
  	for (int i = 0; i < 256; i++) {
		crc = (i << 24); /* Put i into MSB */
	    	for(int j = 0; j < 8; j++) /* 8 reductions */
	      		crc = (crc << 1) ^ 
				((crc & 0x80000000L) ? generator : 0);
	    	crcTable[i] = crc;
	}
}

unsigned int lib_crc32_compute(unsigned char msgByte, unsigned int crc)
{
	crc = crcTable[((crc ^ reverse(msgByte)) >> 24)] ^ (crc << 8);
	return crc;
}

unsigned int lib_crc32_finalise(unsigned int crc)
{
	crc = reverse(crc);
	return crc ^ ~0;
}
