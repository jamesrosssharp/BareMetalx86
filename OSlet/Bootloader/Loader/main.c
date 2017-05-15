
#include "Console/console.h"
#include "BIOS/bios.h"
#include "IO/io.h"
#include "Gfx/gfx.h"
#include "Mem/mem.h"

#include "loaderui.h"

extern int _low_mem_start;
extern int _low_mem_end;

extern int _data_start;
extern int _data_end;

extern int _bss_start;
extern int _bss_end;

void main(void)
{

	// Get BIOS data area.

	struct BiosDataArea* bda = bios_getBDA();

	unsigned short videoPort = bda->videoIOBase;
	unsigned short columns   = bda->textModeColumns;

	// Set up a video console

	console_init();

	console_addConsole(CONSOLETYPE_VGATEXT, videoPort, columns);

	kprintf("Hello from Loader...\n");

	io_detectCPU();

	mem_initHimem();

	gfx_detectVESAModes();

	int xres = GFX_XRESOLUTION_MAX;
	int yres = GFX_YRESOLUTION_MAX;
	int bpp =  GFX_BPP_MAX;
	int mode = 0;

	if (! gfx_vesa_findCompatibleMode(&mode, &xres, &yres, &bpp))
	{
		kprintf("Could not set video mode\n");
		goto die;
	}

	// set up interrupts, to trap exceptions
	// TODO: Move this earlier, and make interrupts
	// properly reloaded after call to real mode vector

	if (! io_initInterrupts(INTERRUPTCONTROL_PIC))
	{
		kprintf("Could not init interrupts.\n");
		goto die;
	}

	io_enableInterrupts();

	// init low mem
	// TODO: this is the first thing we should do

	kprintf("Low mem start: %08x end: %08x\n", &_low_mem_start, &_low_mem_end);
	kprintf("Data start: %08x end: %08x\n", &_data_start, &_data_end);
	kprintf("BSS start: %08x end: %08x\n", &_bss_start, &_bss_end);

	if (! mem_initLowmem(&_low_mem_start, (uintptr_t)&_low_mem_end - (uintptr_t)&_low_mem_start))
	{
		kprintf("Could not init low mem!\n");
		goto die;
	}

	//

/*	kprintf("Using video mode: %d x %d, %d bpp\n", xres, yres, bpp);

	struct FrameBuffer* fb = gfx_vesa_createFrameBuffer(mode);	

	if (fb == NULL)
	{
		kprintf("Could not create a frameBuffer object\n");
		goto die;
	}

	kprintf("created framebuffer: %08x\n", fb);

	if (! fb->activateFrameBufferDisplay(fb))
	{
		kprintf("Couldn't set video mode\n");
		goto die;
	}

	loaderUI_init(fb);
*/
	// 0. enable interrupts

	// 1. init BIOS disk driver

	// 2. init FAT filesystem

	// 3. read BOOT/BOOTARGS.TXT

	// 4. Read kernel to 0x100000

	// 5. jump to kernel

	kprintf("Done with everything!\n");
die:
	asm("hlt");
	goto die;
}
