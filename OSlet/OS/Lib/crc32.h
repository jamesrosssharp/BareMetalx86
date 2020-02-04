

enum CRC32Generator
{
	CRC32 	= 0x04C11DB7,
	CRC32C 	= 0x1EDC6F41,
	CRC32K 	= 0x741B8CD7,
	CRC32K2 = 0x32583499,
	CRC32Q  = 0x814141AB,
};


void lib_crc32_init(enum CRC32Generator generator);
unsigned int lib_crc32_compute(unsigned char msgByte, unsigned int crc);
unsigned int lib_crc32_finalise(unsigned int crc);
