
#include "biosblock.h"
#include "../../Mem/mem.h"
#include "../../IO/io.h"
#include "../../Console/console.h"

static int readSectors(struct BlockDevice* bdev, void* dest, unsigned long long int offset, unsigned int nsectors)
{

	struct BiosBlockDevice* dev = (struct BiosBlockDevice*)bdev;

	struct RegisterDescription in = {0};
	struct RegisterDescription out = {0};

	if (dev->biosExtensionsPresent)
	{

		unsigned char* destBuf = (unsigned char*)dest;

		unsigned int sectorsRead = 0;

		while (nsectors)
		{
			unsigned int sectorsToRead = nsectors > BIOSBLOCKDEVICE_BUFFERSECTORS ? BIOSBLOCKDEVICE_BUFFERSECTORS : nsectors;
			unsigned int bytesToRead = sectorsToRead * dev->blockDev.bytesPerSector;	

			// Set up dap

			dev->biosDAP->size = 0x10;
			dev->biosDAP->nSectors = sectorsToRead; 		
			dev->biosDAP->dataOffset = (uintptr_t)dev->sectorBuffer & 0xffff;
			dev->biosDAP->dataSegment = ((uintptr_t)dev->sectorBuffer & 0xffff0000) >> 4;
			dev->biosDAP->absoluteSectorOffset = offset + sectorsRead;

			in.EAX = 0x4200;
			in.EDX = (unsigned int)dev->driveNumber;
	
			// DS:SI points to buffer
			in.DS  = (((uintptr_t)dev->biosDAP) & 0xffff0000) >> 4;
			in.ESI = (((uintptr_t)dev->biosDAP) & 0x0000ffff); 

			if (! io_realModeInt(0x13, &in, &out))
			{
				kprintf("BiosBlockDevice: call to read sectors extended failed!\n");
				return sectorsRead;
			}

			// copy to dest	

			lib_memcpy(destBuf, dev->sectorBuffer, bytesToRead);

			nsectors -= sectorsToRead;
			sectorsRead += sectorsToRead;
			destBuf += bytesToRead;		

		}

		return sectorsRead;

	}
	else
	{

		unsigned int cyl = 0;
		unsigned int head = 0;
		unsigned int sector = 0;

		// check if end is outside disk
		if (! devices_block_absoluteSectorsToCHS(offset + nsectors, &cyl, &head, &sector, 
							dev->cylinders, dev->headsPerCylinder, dev->sectorsPerHead))
			return 0;

		// check if offset is outside disk (and convert to CHS)
		if (! devices_block_absoluteSectorsToCHS(offset, &cyl, &head, &sector, 
							dev->cylinders, dev->headsPerCylinder, dev->sectorsPerHead))
			return 0;

		unsigned char* destBuf = (unsigned char*)dest;
		unsigned int sectorsRead = 0;

		while (nsectors)
		{

			unsigned int sectorsToRead = nsectors > BIOSBLOCKDEVICE_BUFFERSECTORS ? BIOSBLOCKDEVICE_BUFFERSECTORS : nsectors;
 			sectorsToRead = sectorsToRead > (dev->sectorsPerHead - (sector - 1)) ? (dev->sectorsPerHead - (sector - 1)) : sectorsToRead;  

			unsigned int bytesToRead = sectorsToRead * dev->blockDev.bytesPerSector;	

			in.EAX = 0x0200 | sectorsToRead;
			in.EDX = (dev->driveNumber & 0xff) | ((head & 0xff) << 8);

			in.ES = ((uintptr_t)dev->sectorBuffer & 0xffff0000) >> 4;
			in.EBX = (uintptr_t)dev->sectorBuffer & 0xffff;

			in.ECX = ((cyl & 0xff) << 8) | (sector & 0x3f) | ((cyl & 0x300) >> 2);

			if (! io_realModeInt(0x13, &in, &out))
			{
				kprintf("BiosBlockDevice: call to read sectors failed!\n");
				return sectorsRead;
			}

			// copy to dest	

			lib_memcpy(destBuf, dev->sectorBuffer, bytesToRead);

			nsectors -= sectorsToRead;
			sectorsRead += sectorsToRead;
			destBuf += bytesToRead;		

			sector += sectorsToRead;

			if (sector > dev->sectorsPerHead)
			{
				sector = 1;
				head ++;
				
				if (head >= dev->headsPerCylinder)
				{
					head = 0;
					cyl ++;
				}
		
			}

		}

		return sectorsRead;

	}

}	
	
static bool readSectorsAsynch(struct BlockDevice* dev, void* dest, unsigned long long int offset, unsigned int nsectors,
					BlockDeviceAsynchCallback callback, void* data)
{
	kprintf("BiosBlockDevice: readSectorsAynch not supported!\n");
	return false;
}
	
struct BiosBlockDevice* devices_biosBlock_createBIOSBlockDevice(unsigned char driveNumber)
{

	struct BiosBlockDevice* device = (struct BiosBlockDevice*) kmalloc(sizeof(struct BiosBlockDevice), MEMORYTYPE_HIMEM);

	device->driveNumber = driveNumber;

	// Query for bytes per sector and drive geometry using BIOS calls

	unsigned int bytesPerSector = 512;
	unsigned int cylinders = 0;
	unsigned int heads = 0;
	unsigned int sectors = 0;

	// check if BIOS extensions are present for given drive	

	struct RegisterDescription in = {0};
	struct RegisterDescription out = {0};

	in.EAX = 0x4100;
	in.EDX = (unsigned int)driveNumber;	
	in.EBX = 0x55aa;

	bool present = io_realModeInt(0x13, &in, &out);	

	present = false;	// used to test non extended BIOS calls

	if (present)
	{
		device->biosExtensionsPresent = true;

		kprintf("BiosBlockDevice: BIOS extensions are present\n");
		
		// allocate lomem for parameter buffer

		struct BiosExtendedDriveParameters* parms = 
			(struct BiosExtendedDriveParameters*)kmalloc(sizeof(struct BiosExtendedDriveParameters), MEMORYTYPE_LOMEM); 

		if (parms == NULL)
		{
			kprintf("BiosBlockDevice: could not allocate lo mem!\n");
			kfree(device);
			return NULL;
		}

		parms->size = 0x1e;
		
		// BIOS extensions are present - get extended parameters

		in.EAX = 0x4800;
		in.EDX = (unsigned int)driveNumber;
	
		// DS:SI points to buffer
		in.DS  = (((uintptr_t)parms) & 0xffff0000) >> 4;
		in.ESI = (((int)parms) & 0x0000ffff); 

		if (! io_realModeInt(0x13, &in, &out))
		{
			kprintf("BiosBlockDevice: call to get extended drive parameters failed!\n");
			kfree(parms);
			kfree(device);	
			return NULL;
		}

		device->physCylinders = parms->physCylinders;
		device->physHeadsPerCylinder = parms->physHeads;
		device->physSectorsPerHead = parms->physSectors;
		device->absoluteSectors = parms->absSectors; 

		bytesPerSector = parms->bytesPerSector; 

		kprintf("Extended parms for drive %02x:\n", device->driveNumber);
		kprintf("C: %d H: %d S: %d BPS: %d\n", device->physCylinders, 
							     device->physHeadsPerCylinder, 
							     device->physSectorsPerHead,
							     bytesPerSector);	

		kfree(parms);

		// allocate DAP structure in low mem

		device->biosDAP = (struct BiosDiskAddressPacket*)kmalloc(sizeof(struct BiosDiskAddressPacket), MEMORYTYPE_LOMEM);

		if (device->biosDAP == NULL)
		{
			kprintf("Couldn't allocate memory for BIOS DAP!\n");
			kfree(device);
			return NULL;
		}

	}
	else
	{
		device->biosExtensionsPresent = false;

		// BIOS extensions are not present - get parameters, and assume default bytes per sector
		kprintf("BiosBlockDevice: extensions not supported!\n");

		in.EAX = 0x0800;
		in.EDX = (unsigned int)driveNumber;
	
		in.ES  = 0;
		in.EDI = 0; 

		if (! io_realModeInt(0x13, &in, &out))
		{
			kprintf("BiosBlockDevice: call to get drive parameters failed!\n");
			kfree(device);	
			return NULL;
		}

		device->cylinders = ((out.ECX & 0xff00) >> 8 | (out.ECX & 0xc0) << 2) + 1;
		device->headsPerCylinder = ((out.EDX & 0xff00) >> 8) + 1;
		device->sectorsPerHead = out.ECX & 0x3f;

		kprintf("Parms for drive %02x:\n", device->driveNumber);
		kprintf("C: %d H: %d S: %d\n", device->cylinders, 
							     device->headsPerCylinder, 
							     device->sectorsPerHead);	

		device->biosDAP = NULL;	// overkill

	}

	if (! devices_block_initBlockDevice((struct BlockDevice*)device, bytesPerSector))
	{
		kprintf("Could not init block device!\n");
		if (device->biosDAP != NULL)
			kfree(device->biosDAP);
		kfree(device);
		return NULL;
	}

	// allocate low memory for sector buffer

	device->sectorBuffer = (unsigned char*)kmalloc(BIOSBLOCKDEVICE_BUFFERSECTORS * device->blockDev.bytesPerSector, MEMORYTYPE_LOMEM);

	if (device->sectorBuffer == NULL)
	{
		kprintf("Could not allocate sector buffer!\n");

		if (device->biosDAP != NULL)
			kfree(device->biosDAP);
		kfree(device);
		return NULL;
	}

	// fill in function pointers

	device->blockDev.readSectors = readSectors;
	device->blockDev.readSectorsAsynch = readSectorsAsynch;

	return device;

}


void	devices_biosBlock_destroyBIOSBlockDevice(struct BiosBlockDevice** dev)
{

	if ((dev == NULL) || (*dev == NULL))
		return;

	if ((*dev)->sectorBuffer != NULL)
		kfree((*dev)->sectorBuffer);

	if ((*dev)->biosDAP != NULL)
		kfree((*dev)->biosDAP);

	kfree(*dev);
	*dev = NULL;

}
