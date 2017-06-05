

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

int lib_strncmp(const char* s1, const char* s2, unsigned int len)
{

	int cmp = 0;
	int cnt = 0;

	while (cnt < len && s1 && s2 && *s1 && *s2)
	{
		cmp += *s1++; 
		cmp -= *s2++; 
	}

	return cmp;

}

int lib_strlen(const char* s)
{
	int len = 0;

	while (*s++)
		len++;

	return len;
}

char lib_toupper(char c)
{
	if (c >= 'a' && c <= 'z')
		c += 'A' - 'a';
	return c;		
}

void lib_strtoupper(char* s)
{
	while (*s)
	{
		*s = lib_toupper(*s);
		s++;
	}
}
