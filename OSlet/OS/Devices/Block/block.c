
#include "block.h"

static int readSectors(struct BlockDevice* dev, void* dest, unsigned long long int offset, unsigned int nsectors)
{
	return 0;
}
	
static bool readSectorsAsynch(struct BlockDevice* dev, void* dest, unsigned long long int offset, unsigned int nsectors,
					BlockDeviceAsynchCallback callback, void* data)
{
	return false;
}


bool devices_block_initBlockDevice(struct BlockDevice* device, unsigned int bytesPerSector)
{

	device->bytesPerSector = bytesPerSector;
		
	device->readSectors = readSectors;
	device->readSectorsAsynch = readSectorsAsynch;


	return true;
}

bool devices_block_absoluteSectorsToCHS(unsigned long long int offset, unsigned int* cyl, 
					unsigned int *head, unsigned int* sectors, 
					unsigned int ncylinders, unsigned int headsPerCylinder, unsigned int sectorsPerHead)
{

	if (offset > 0xffffffff)	// Greater than 32 bits cannot be respresented in 3 bytes anyway, so no need for
		return false;		// 64-bit divides and modulo

	unsigned int ofs = (unsigned int)offset;

	unsigned int s = ofs % (sectorsPerHead) + 1;
        unsigned int h = (ofs / sectorsPerHead) % headsPerCylinder;
        unsigned int c = ((ofs / sectorsPerHead) / headsPerCylinder);

	if (c >= ncylinders)
		return false;

	*cyl = c;
	*head = h;
	*sectors = s;

	return true;
}

