
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

