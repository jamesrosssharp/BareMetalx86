
#include "port.h"
#include "../Console/console.h"

void outByte(unsigned short port, char byte)
{
  	__asm__ volatile (
            " out %%al, %%dx \n"
                :
                : "a" ((unsigned int)byte),
                  "d" ((unsigned int)port)
            );

}

char inByte(unsigned short port)
{

	char inByte = 0;

  	__asm__ volatile (
            " in %%dx, %%al \n"
                : "=a" (inByte)
                : "d" (port)
		:
            );

	return inByte;
}

void outLong(unsigned short port, long unsigned int val)
{
  	__asm__ volatile (
            " out %%eax, %%dx \n"
                :
                : "a" ((unsigned int)val),
                  "d" ((unsigned int)port)
            );

}

long unsigned int inLong(unsigned short port)
{

	long unsigned int in = 0;

  	__asm__ volatile (
            " in %%dx, %%eax \n"
                : "=a" (in)
                : "d" (port)
		:
            );

	return in;
}

