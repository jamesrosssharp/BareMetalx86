
#pragma once

#include "io.h"


#define IDT_GATE_TYPE_386_TASK_GATE		0x05
#define IDT_GATE_TYPE_286_INTERRUPT_GATE 	0x06
#define IDT_GATE_TYPE_286_TRAP_GATE		0x07
#define IDT_GATE_TYPE_386_INTERRUPT_GATE	0x0e
#define IDT_GATE_TYPE_386_TRAP_GATE		0x0f

struct __attribute__((packed)) IDTEntry 
{
	unsigned short offsetLo;
	unsigned short selector;
	unsigned char  zero;
	unsigned char  type;
	unsigned short offsetHi;
};

typedef void (* ISRCallback)(int interrupt, int errorCode);

