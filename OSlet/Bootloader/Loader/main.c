
#include "Console/console.h"
#include "BIOS/bios.h"
#include "IO/io.h"
#include "Gfx/gfx.h"
#include "Mem/mem.h"

#include "loaderui.h"

void main(void)
{

	// Get BIOS data area.

	struct BiosDataArea* bda = bios_getBDA();

	unsigned short videoPort = bda->videoIOBase;
	unsigned short columns   = bda->textModeColumns;

	// Set up a video console

	console_init();

	io_detectCPU();

	mem_init();

	console_addConsole(CONSOLETYPE_VGATEXT, videoPort, columns);

	kprintf("Hello from Loader...\n");

	gfx_detectVESAModes();

	int xres = 800; //GFX_XRESOLUTION_MAX;
	int yres = 600; //GFX_YRESOLUTION_MAX;
	int bpp = 32; //GFX_BPP_MAX;
	int mode = 0;

	if (! gfx_vesa_findCompatibleMode(&mode, &xres, &yres, &bpp))
		goto die;

	kprintf("Using video mode: %d x %d, %d bpp\n", xres, yres, bpp);

	struct FrameBuffer* fb = gfx_vesa_createFrameBuffer(mode);	

	if (fb == NULL)
	{
		kprintf("Could not create a frameBuffer object\n");
		goto die;
	}

	if (! fb->activateFrameBufferDisplay(fb))
	{
		kprintf("Couldn't set video mode\n");
		goto die;
	}

	loaderUI_init(fb);

	// 0. enable interrupts

	// 1. init BIOS disk driver

	// 2. init FAT filesystem

	// 3. read BOOT/BOOTARGS.TXT

	// 4. Read kernel to 0x100000

	// 5. jump to kernel


die:
	asm("hlt");
	goto die;
}
