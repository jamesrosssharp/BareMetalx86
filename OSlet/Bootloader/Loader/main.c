
#include "Console/console.h"
#include "BIOS/bios.h"
#include "IO/io.h"
#include "Gfx/gfx.h"
#include "Mem/mem.h"

#include "loaderui.h"

#include "Devices/Block/block.h"

extern int _low_mem_start;
extern int _low_mem_end;

extern int _data_start;
extern int _data_end;

extern int _bss_start;
extern int _bss_end;

extern int boot_driveNumber;

void main(void)
{

	// init low mem

	if (! mem_initLowmem(&_low_mem_start, (uintptr_t)&_low_mem_end - (uintptr_t)&_low_mem_start))
	{
		// can't print a message yet.
		goto die;
	}

	// Get BIOS data area.

	struct BiosDataArea* bda = bios_getBDA();

	unsigned short videoPort = bda->videoIOBase;
	unsigned short columns   = bda->textModeColumns;

	// Set up a video console

	console_init();

	console_addConsole(CONSOLETYPE_VGATEXT, videoPort, columns);

	kprintf("Hello from Loader...\n");

	io_detectCPU();

	if (! mem_initHimem())
	{
		kprintf("Could not init himem!\n");
		goto die;
	}

	// Now create a block device instance for the drive we got from the bootloaders

	struct BiosBlockDevice* bdev = devices_biosBlock_createBIOSBlockDevice((unsigned char)boot_driveNumber); 	

	if (bdev == NULL)
	{
		kprintf("Could not create BIOS block device!\n");
		goto die;
	}

	unsigned char buf[4096];

	unsigned int sectors = bdev->blockDev.readSectors((struct BlockDevice*)bdev, buf, 0, 1);  

	if (sectors != 1)
	{
		kprintf("Could not read sectors!\n");
		goto die;
	}

	unsigned int fsType = buf[0x1be + 4];
	unsigned int cs	    = buf[0x1be + 2];
	unsigned int c	    = buf[0x1be + 3];
	unsigned int h	    = buf[0x1be + 1];

	unsigned int lba    = *(unsigned int*)&buf[0x1be + 8];

	kprintf("MBR 0: FS=%x Start C: %d H: %d S: %d LBA: %08x\n", 
			fsType,
			c | (cs & 0xc0)<<2,
			h,
			cs & 0x3f,
			lba);

	goto die;

	// Set up the VFS


	// Set up devFS


	// Add bios block device

	// Create partitions for bios block device and add to devFS 


	// Search for boot partition


	// Mount boot partition


	// Parse BOOTCONF.TXT


	// Detect video modes
	
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

die:
	kprintf("Done with everything!\n");
hlt:
	asm("hlt");
	goto hlt;
}
