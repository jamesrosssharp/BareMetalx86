
#include "Console/console.h"
#include "BIOS/bios.h"

void main(void)
{

	// Get BIOS data area.

	struct BiosDataArea* bda = bios_getBDA();

	unsigned short videoPort = bda->videoIOBase;
	unsigned short columns   = bda->textModeColumns;


	// Set up a video console

	console_init();

	console_addConsole(CONSOLETYPE_VGATEXT, videoPort, columns);

	bios_printBDA();


die:
	asm("hlt");
	goto die;
}
