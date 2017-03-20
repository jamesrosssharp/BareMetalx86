
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

	// Detect the memory map

	int memMapSize = 0;
	struct MemoryEntry* memMap = 0; 

	bios_detectMemory(&memMap, &memMapSize);

	// Detect VESA modes

	gfx_detectVESAModes();

	// Remap interrupt controller, install interrupt handlers
	// and enable interrupts

	io_initAndRemapPIC();

die:
	asm("hlt");
	goto die;
}
