
#include "port.h"

void outByte(unsigned short port, char byte)
{
  __asm__ volatile (
            " push %%eax \n"
            " push %%edx \n"
            " mov %%eax, %0 \n" // these instructions are quite irrelevant 
            " mov %%edx, %1 \n" // if you look at the disassembly.
            " out %%al, %%dx \n"
            " pop %%edx \n"
            " pop %%eax \n"
                :
                : "r" ((unsigned int)byte),
                  "r" ((unsigned int)port)
            );
}

