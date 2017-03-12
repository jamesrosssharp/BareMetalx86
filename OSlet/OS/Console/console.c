/*

	console.c: top level for consoles

						*/


#include <stdarg.h>

#include "vgaconsole.h"
#include "console.h"

#include "../kernerr.h"

#define MAX_CONSOLES	4	

struct ConsoleInfo* gConsoles[MAX_CONSOLES];
int 	     gNumConsoles;

int console_init()
{

	gNumConsoles = 0;

	return 0;
}

int console_addConsole(enum ConsoleType type, ...)
{

	va_list arg;
	va_start(arg, type);

	if (gNumConsoles >= MAX_CONSOLES)
		return ERROR_NO_MEMORY;

	struct ConsoleInfo *info = NULL; 

	switch(type)
	{
		case CONSOLETYPE_VGATEXT: 
		{

			int port = va_arg(arg, int);

			int columns = va_arg(arg, int);  

			info = vgaConsole_initVGAConsole(port, columns);	
			break;
		}
		default:
		{
			va_end(arg);
			return ERROR_INVALID_PARAMETER;
			break;
		}
	}

	info->init();

	gConsoles[gNumConsoles ++] = info;


	va_end(arg);

	return 0;
}

void console_clear()
{

	for (int i = 0; i < gNumConsoles; i++)
	{
		gConsoles[i]->clear();
	}
}

void console_putch(char c)
{
	for (int i = 0; i < gNumConsoles; i++)
	{
		gConsoles[i]->putch(c);
	}

}

void console_puts(const char* s)
{
	for (int i = 0; i < gNumConsoles; i++)
	{
		gConsoles[i]->puts(s);
	}
}

