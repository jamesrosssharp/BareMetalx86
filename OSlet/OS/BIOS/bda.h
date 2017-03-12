
#pragma once

#include "../kerndefs.h"
#include "../Console/console.h"

/*

	From OSDev Wiki:



0x0400 (4 words) 	IO ports for COM1-COM4 serial (each address is 1 word, zero if none)
0x0408 (3 words) 	IO ports for LPT1-LPT3 parallel (each address is 1 word, zero if none)
0x040E (word) 	EBDA base address >> 4 (usually!)
0x0410 (word) 	packed bit flags for detected hardware
0x0417 (word) 	keyboard state flags
0x041E (32 bytes) 	keyboard buffer
0x0449 (byte) 	Display Mode
0x044A (word) 	number of columns in text mode
0x0463 (2 bytes, taken as a word) 	base IO port for video
0x046C (word) 	# of IRQ0 timer ticks since boot
0x0475 (byte) 	# of hard disk drives detected
0x0480 (word) 	keyboard buffer start
0x0482 (word) 	keyboard buffer end
0x0497 (byte) 	last keyboard LED/Shift key state 

				*/


struct __attribute__((__packed__)) BiosDataArea
{
	unsigned short 	COM1Port;
	unsigned short 	COM2Port;
	unsigned short 	COM3Port;
	unsigned short 	COM4Port;

	unsigned short 	LPT1Port;
	unsigned short 	LPT2Port;
	unsigned short 	LPT3Port;

	unsigned short 	EBDAAddress;
	unsigned short 	hardwareFlags;

	char	pad1[5];

	unsigned short 	keyboardStateFlags;

	char	pad2[5];

	unsigned char	keyboardBuffer[32];

	char	pad3[11];

	unsigned char 	displayMode;
	unsigned short  textModeColumns;

	char 	pad4[23];

	unsigned short  videoIOBase;

	char 	pad5[7];

	unsigned short  ticksSinceBoot;

	char	pad6[7];

	unsigned char  numberOfHarddisksDetected;

	char	pad7[89];

};



struct	BiosDataArea*	bios_getBDA();
void	bios_printBDA();

