
#pragma once

#include "io.h"


#define IDT_GATE_TYPE_386_TASK_GATE		0x05
#define IDT_GATE_TYPE_286_INTERRUPT_GATE 	0x06
#define IDT_GATE_TYPE_286_TRAP_GATE		0x07
#define IDT_GATE_TYPE_386_INTERRUPT_GATE	0x0e
#define IDT_GATE_TYPE_386_TRAP_GATE		0x0f

#define IDT_MAKE_TYPE(type, present, dpl, storageSeg) \
			type | ((present&1) << 7) | \
			((dpl & 3) << 5) | \
			((storageSeg & 1) << 4)

struct __attribute__((packed)) IDTEntry 
{
	unsigned short offsetLo;
	unsigned short selector;
	unsigned char  zero;
	unsigned char  type;
	unsigned short offsetHi;
};

enum InterruptControlMode
{
	INTERRUPTCONTROL_NOINIT,
	INTERRUPTCONTROL_PIC,
	INTERRUPTCONTROL_APIC
};

typedef void (* ISRCallback)(int interrupt, int errorCode);

bool io_initInterrupts(enum InterruptControlMode interruptControl);
void io_enableInterrupts();
void io_disableInterrupts();
