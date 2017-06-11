
#include "Console/console.h"
#include "BIOS/bios.h"
#include "IO/io.h"
#include "Gfx/gfx.h"
#include "Mem/mem.h"

#include "loaderui.h"
#include "bootopts.h"

#include "Devices/Block/block.h"
#include "Devices/Block/mbrpartition.h"

#include "Filesystems/fs.h"

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

	struct PartitionBlockDevice** partitions;
	unsigned int nPartitions = 0;

	devices_mbrPartition_createPartitionDevices((struct BlockDevice*) bdev, &partitions, &nPartitions);

	kprintf("Created %d partitions\n", nPartitions);

	bool mountedPartition = false;
	
	struct FATFileSystem* fatfs; 

	for (int i = 0; i < nPartitions; i++)
	{
		struct PartitionBlockDevice* p = partitions[i];

		if (p->partitionType == 0x06)
		{
			kprintf("Found FAT partition: %d %08llx\n", p->partitionNumber, p->startSector);
		
			fatfs = fs_fat_createFATFileSystem((struct BlockDevice*)p);  	

			if (fatfs != NULL)
			{

				// Get compare volume label to loader partition string

				
				if (lib_strncmp(fatfs->volumeLabel, "OSLETLOADER", 11) == 0)
				{
					kprintf("Found boot partition!\n");
					mountedPartition = true;
				}

			} 
			else
			{
				kprintf("fatfs was null!\n");
			}
	
		}
	}

	if (! mountedPartition)
	{
		kprintf("Could not mount boot partition!\n");
		goto die;
	}

	struct UnicodeString* fname = lib_createUnicodeString();

	fname->appendASCIICString(fname, "/boot/bootopts.txt");

	struct File* bootText = ((struct FileSystem*)fatfs)->open((struct FileSystem*)fatfs, fname, O_RDONLY);

	if (bootText == NULL)
	{
		kprintf("Could not open boot config file...\n");
		goto die;
	}

	unsigned int textSize = bootText->size;

	kprintf("Boot options file size: %d bytes\n", textSize);

	if (textSize == 0)
	{
		kprintf("Boot options file was zero bytes!\n");
		goto die;
	}

	char* text = (char*)kmalloc(textSize + 1, MEMORYTYPE_HIMEM);

	if (bootText->read(bootText, text, textSize) != textSize)
	{
		kprintf("Could not read text file\n");
		goto die;
	}

	// Parse BOOTCONF.TXT

	struct BootOpts bootOpts;

	parseBootOpts(&bootOpts, text, text + textSize);
	
	kfree(text);


	// Detect video modes
	
	gfx_detectVESAModes();

	int xres = bootOpts.bootXRes;
	int yres = bootOpts.bootYRes;
	int bpp =  bootOpts.bootBpp;
	int mode = 0;

	if (! gfx_vesa_findCompatibleMode(&mode, &xres, &yres, &bpp))
	{
		kprintf("Could not set video mode\n");
		goto die;
	}

	//

	kprintf("Using video mode: %d x %d, %d bpp\n", xres, yres, bpp);


	
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

	// 4. Read kernel to 0x100000

	

	// 5. jump to kernel

die:
	kprintf("Done with everything!\n");
hlt:
	asm("hlt");
	goto hlt;
}
