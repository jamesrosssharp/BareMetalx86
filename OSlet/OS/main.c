
#include "Console/console.h"
#include "BIOS/bios.h"
#include "IO/io.h"
#include "Gfx/gfx.h"

void main(void)
{

	// Get BIOS data area.

	struct BiosDataArea* bda = bios_getBDA();

	unsigned short videoPort = bda->videoIOBase;
	unsigned short columns   = bda->textModeColumns;


	// Set up a video console

	console_init();

	console_addConsole(CONSOLETYPE_VGATEXT, videoPort, columns);

	// Print the BIOS data area
	
	bios_printBDA();

	// Detect the CPU
	
	io_detectCPU();

	// Init memory subsystem

	//int memMapSize = 0;
	//struct MemoryEntry* memMap = 0; 

	//bios_detectMemory(&memMap, &memMapSize);

	mem_init();

	// Now we have detected memory, init the kernel memory allocator,
	// so we can kmalloc

	// Detect VESA modes

	//gfx_detectVESAModes();

	// Now we have detected the vesa modes, find the highest resolution
	// mode and set it; init the vesa framebuffer driver; init the 
	// graphics console. If we can't find a suitable graphical mode,
	// continue in text mode.


	
	// init interrupts using 8259 PIC interrupt system

	//if (! io_initInterrupts(INTERRUPTCONTROL_PIC))
	//{
	//	kprintf("Could not init interrupts.\n");
	//	goto die;
	//}

	// init keyboard driver and install its interrupt callback

	// init the mouse driver and install its interrupt callback

	// enable interrupts

	//io_enableInterrupts();

	// Now we have a graphical console (hopefully) and a functioning 
	// events sub system, we can start to detect hardware and log
	// the start up. 

	// Detect ACPI. 

		// Do we need to control the fan?

		// Do we have a battery? 


	// Scan PCI bus

	//io_probePCIBus();

	// Did we find a ATA/SATA AHCI? If so, add disks

	// Did we find a USB host device? If so, init the USB subsystem

		// Scan and enumerate USB buses

	// Did we find a graphics card?

	// Did we find a sound card?

	//
	//	Now we have detected an initialised all supported hardware,
	//	we want to start the scheduler
	//		


	// 	Launch init process, which will

	
	// 	Jump to init process.


	// 	Shouldn't get here
die:
	asm("hlt");
	goto die;
}
