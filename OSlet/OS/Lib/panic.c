

#include "panic.h"
#include "../Console/console.h"

void	kpanic(char* msg)
{

	kprintf(msg);	

die:
	asm("hlt");
	goto die;

}

