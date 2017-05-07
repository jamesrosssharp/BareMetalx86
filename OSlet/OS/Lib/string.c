

void lib_bzero(void* mem, int bytes)
{

	unsigned char* ptr = (unsigned char*)mem;
	for (int i = 0; i < bytes; i++)
		ptr[i] = 0;

}

void lib_memcpy(void* dest, void* src, unsigned int bytes) // TODO: should be size_t...
{

	unsigned char* destBytes = dest;
	unsigned char* srcBytes = src;

	unsigned int alignBytes = bytes & 0x3;

	bytes -= alignBytes;

	while (alignBytes--)
		*destBytes++ = *srcBytes++;


	while (bytes >= 4)
	{
		*(unsigned int*)destBytes = *(unsigned int*)srcBytes;
		destBytes += 4;
		srcBytes  += 4;
		bytes -= 4;
	}

	while (bytes--)
		*destBytes++ = *srcBytes++;

}
