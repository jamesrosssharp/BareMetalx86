
#include "bda.h"


struct	BiosDataArea*	bios_getBDA()
{
	return (struct BiosDataArea*)0x400;
}

void	bios_printBDA()
{

	kprintf("======== BIOS Data Area =======\n");

	struct BiosDataArea* bda = bios_getBDA();

	kprintf("COMports: 0x%04x 0x%04x 0x%04x 0x%04x\n", bda->COM1Port, bda->COM2Port, bda->COM3Port, bda->COM4Port);

	kprintf("LPTports: 0x%04x 0x%04x 0x%04x\n", bda->LPT1Port, bda->LPT2Port, bda->LPT3Port);

	kprintf("EBDA Address: 0x%04x\n", bda->EBDAAddress);

	kprintf("Hardware flags: 0x%04x\n", bda->hardwareFlags);

	kprintf("Display mode: 0x%02x\n", bda->displayMode);

	kprintf("Columns: 0x%04x\n", bda->textModeColumns);

	kprintf("Video IO Base: 0x%04x\n", bda->videoIOBase);

	kprintf("Ticks since boot: %d %x\n", bda->ticksSinceBoot, &bda->ticksSinceBoot);

	kprintf("Number of harddisks detected: %d %x\n", bda->numberOfHarddisksDetected, &bda->numberOfHarddisksDetected);

	kprintf("================================\n");

}

