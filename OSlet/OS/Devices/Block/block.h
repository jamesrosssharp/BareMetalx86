
#pragma once

#include "../../kerndefs.h"

typedef void (*BlockDeviceAsynchCallback)(void* data);

struct BlockDevice
{

	unsigned int 		bytesPerSector;

	// blocking (synchronous) read
	// Returns number of sectors read
	int (*readSectors)(struct BlockDevice* dev, void* dest, unsigned long long int offset, unsigned int nsectors); 	
	
	// non-blocking (queued) read
	// Returns true if read could be queued
	bool (*readSectorsAsynch)(struct BlockDevice* dev, void* dest, unsigned long long int offset, unsigned int nsectors,
					BlockDeviceAsynchCallback callback, void* data);
					

};

bool devices_block_initBlockDevice(struct BlockDevice* device, unsigned int bytesPerSector);
bool devices_block_absoluteSectorsToCHS(unsigned long long int offset, unsigned int* cyl, 
					unsigned int *head, unsigned int* sectors, 
					unsigned int ncylinders, unsigned int headsPerCylinder, unsigned int sectorsPerHead);


#include "biosblock.h"

