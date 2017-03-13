
#include "console.h"
#include "../IO/io.h"


/*	Defines			*/

#define CONSOLE_COLOR	0x0f

#define VIDEO_MEM		0xb8000

/*	Funcion prototypes	*/

int vgaConsole_init(void);
void vgaConsole_clear(void);
void vgaConsole_putch(char c);
void vgaConsole_puts(const char* str);

/*	Globals		*/

struct ConsoleInfo gConsoleInfo = {
	
		vgaConsole_init,	// int init(void)
		vgaConsole_clear,	// void clear()
		vgaConsole_putch,	// void putch(char c)
		vgaConsole_puts		// void puts(const char* str)	

	};

int	gConsoleRow;
int	gConsoleCol;

int 	gConsoleWidth = 80;
int	gConsoleHeight = 25;

int	gConsolePort	= 0x3d4;

/*		Functions	*/

struct ConsoleInfo* vgaConsole_getConsoleInfo()
{
	return &gConsoleInfo;
}

struct ConsoleInfo* vgaConsole_initVGAConsole(int port, int columns)
{
	gConsoleWidth = columns;
	gConsolePort  = port;

	return &gConsoleInfo;
}

int vgaConsole_init(void)
{
	vgaConsole_clear();

	gConsoleRow = 0;
	gConsoleCol = 0;

	//vgaConsole_puts("VGA Console initialised.\n");
	kprintf("VGA Console at %x\n", gConsolePort);

	return 0;

}

void vgaConsole_clear(void)
{
	char *vidMem = (char*)VIDEO_MEM;
	for (int i = 0; i < gConsoleWidth*gConsoleHeight*2; i ++)
		vidMem[i] = 0x00;
}

void vgaConsole_putch(char c)
{

	if (c == '\n')
	{
		gConsoleCol = 0;
		gConsoleRow ++;
		if (gConsoleRow > gConsoleHeight)
			gConsoleRow = 0;


	}
	else
	{
		char *vidMem = (char*)VIDEO_MEM;

		int offset = (gConsoleCol + gConsoleRow*gConsoleWidth) << 1;
 
		vidMem[offset] = c;
		vidMem[offset + 1] = CONSOLE_COLOR;

		unsigned short position = offset >> 1;
 
    		// cursor LOW port to vga INDEX register
    		outByte(0x3d4, 0x0F);
    		outByte(0x3d4 + 1, (unsigned char)(position&0xFF));
    		// cursor HIGH port to vga INDEX register
    		outByte(0x3d4, 0x0E);
    		outByte(0x3d4 + 1, (unsigned char )((position>>8)&0xFF));
	
		gConsoleCol ++;

		if (gConsoleCol > gConsoleWidth)
		{
			gConsoleCol = 0;
			gConsoleRow ++;

			if (gConsoleRow > gConsoleHeight)
			{
		  		gConsoleRow = 0;
			}
		}
	}


}

void vgaConsole_puts(const char* str)
{
	while (*str != 0)
	{
		vgaConsole_putch(*str);
		str ++;
	}
}
