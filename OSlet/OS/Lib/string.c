
#include "string.h"

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

	while (len-- && s1 && s2)
	{
	
		if (*s1 != *s2)
			return *s1 - *s2;

		if (*s1 == 0 || *s2 == 0)
			break;

		s1++;
		s2++;

	}

	return 0;

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

bool lib_isspace(char c)
{
	return (c == '\n' || c == ' ' || c == '\t' || c == '\r');
}

void lib_trim(char** s)
{

	char *start = *s, *end = *s;

	while (lib_isspace(*start) && *start)
		start ++;

	end = start;

	while (*end)
		end++;

	end--;
	while (lib_isspace(*end) && end > start)
		end --;

	end++;
	*end = '\0';

	*s = start;
}

bool lib_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

int lib_atoi(char *s)
{
	bool negative = false;

	int num = 0;

	while (*s && lib_isspace(*s))
		s++;

	if (*s == '-')
	{
		s++;
		negative = true;
	}

	while (*s && lib_isspace(*s))
		s++;

	while(*s && lib_isdigit(*s))
	{	
		num *= 10;
		num += *s - '0';
		s++;
	}

	if (negative)
		num = -num;

	return num;
}
