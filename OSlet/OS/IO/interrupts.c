
#include "interrupts.h"

#define MAX_HANDLERS_PER_VECTOR 8
#define INTERRUPT_VECTORS 256

struct IDTEntry idtEntries[INTERRUPT_VECTORS] = {0};


extern int boot_interruptJumpTable;	// dummy type declaration (int)

// 
//	Prepare IDT and jump table offsets, but do not enable interrupts.
//	
bool io_initInterrupts()
{
	// init the idt

	
	
}

void io_enableInterrupts()
{

}

void io_disableInterrupts()
{

}

bool io_addInterruptHandler(unsigned char interrupt, ISRCallback handler)
{
	// Set an interrupt handler
}

void io_handleInterrupt(int interrupt, int errorCode)
{

	kprintf("Interrupt: %d %d\n", interrupt, errorCode);

}

