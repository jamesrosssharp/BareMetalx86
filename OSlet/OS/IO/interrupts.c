
#include "interrupts.h"
#include "../Console/console.h"

#define MAX_HANDLERS_PER_VECTOR 8
#define INTERRUPT_VECTORS 256

struct IDTEntry idtEntries[INTERRUPT_VECTORS] __attribute__ ((aligned)) = {0};


extern int boot_interruptJumpTable;	// dummy type declaration (int)
extern void boot_loadIDT(void* IDT);
extern void boot_enableInterrupts();
extern void boot_disableInterrupts();

enum	InterruptControlMode	gInterruptControlMode = INTERRUPTCONTROL_NOINIT;

// 
//	Prepare IDT and jump table offsets, but do not enable interrupts.
//	
bool io_initInterrupts(enum InterruptControlMode interruptControl)
{
	
	gInterruptControlMode = interruptControl;

	if (interruptControl == INTERRUPTCONTROL_PIC)
	{
		// Initialise PIC and remap it.
		io_initAndRemapPIC();	
	}
	else
	{
		return false;
	}


	// init the idt

	kprintf("Init'ing IDT...\n");	
	
	for (int i = 0; i < INTERRUPT_VECTORS; i ++)
	{
		// There are 20 6 byte entries in the jump table,
		// aligned to 128 bytes
		int offset = (int)(&boot_interruptJumpTable) 
					+ (i / 20)*128 + (i%20)*6;

		idtEntries[i].offsetLo = offset & 0x0000ffff;
		idtEntries[i].offsetHi = (offset & 0xffff0000) >> 16;

		idtEntries[i].selector = 0x08;	// kernel code segment
		idtEntries[i].type = IDT_MAKE_TYPE(
					IDT_GATE_TYPE_386_INTERRUPT_GATE,
					1, 0, 0); 	

	}

	boot_loadIDT(idtEntries);

	return true;
}

void io_enableInterrupts()
{
	boot_enableInterrupts();
}

void io_disableInterrupts()
{
	boot_disableInterrupts();
}

bool io_addInterruptHandler(unsigned char interrupt, ISRCallback handler)
{
	// Set an interrupt handler
}

void io_handleInterrupt(int errorCode, int interrupt)
{

	if (interrupt != 0x20)
		kprintf("Interrupt: %x %x\n", interrupt, errorCode);

	if (gInterruptControlMode == INTERRUPTCONTROL_PIC)
	{
		io_acknowledgeInterruptPIC(interrupt);
	}	

	if (interrupt == 0x21)
		inByte(0x60);

}

