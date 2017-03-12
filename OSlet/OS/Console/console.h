
#pragma once

#include "../kerndefs.h"
#include "vgaconsole.h"
#include "kprintf.h"

enum  ConsoleType {
	CONSOLETYPE_VGATEXT,
	CONSOLETYPE_UART,
     };	


// We use this struct to "fake" object oriented programming in C.
// It consists of a lot of function pointers that give us the individual
// console methods for each console (e.g vgaconsole - text mode console,
// serial console - UART based serial console etc.)

struct	ConsoleInfo
{
	int (*init)(void);
	void (*clear)(void);
	void (*putch)(char c);	
	void (*puts)(const char* s);
};

 
int console_init();
void console_clear();
void console_putch(char c);
void console_puts(const char* s);

int console_addConsole(enum ConsoleType type, ...);
