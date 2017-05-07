

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

	for (int i = 0; i < bytes; i++)
		*destBytes++ = *srcBytes++;

}
