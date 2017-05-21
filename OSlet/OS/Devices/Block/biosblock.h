
#pragma once

#include "block.h"

// BIOS Extended Drive Parameters
// 00h..01h 	2 bytes 	size of Result Buffer (set this to 1Eh)
// 02h..03h 	2 bytes 	information flags
// 04h..07h 	4 bytes 	physical number of cylinders = last index + 1 (because index starts with 0)
// 08h..0Bh 	4 bytes 	physical number of heads = last index + 1 (because index starts with 0)
// 0Ch..0Fh 	4 bytes 	physical number of sectors per track = last index (because index starts with 1)
// 10h..17h 	8 bytes 	absolute number of sectors = last index + 1 (because index starts with 0)
// 18h..19h 	2 bytes 	bytes per sector
// 1Ah..1Dh 	4 bytes 	optional pointer to Enhanced Disk Drive (EDD) configuration parameters

struct __attribute__ ((packed)) BiosExtendedDriveParameters
{
	unsigned short 	size;
	unsigned short 	flags;
	unsigned int   	physCylinders;
	unsigned int   	physHeads;
	unsigned int   	physSectors;
	unsigned long long int   absSectors;
	unsigned short 	bytesPerSector;
	void*		eddConfigParameters;
};


// 	00h 	 1 byte 	size of DAP (set this to 10h)
//	01h 	 1 byte 	unused, should be zero
//	02h..03h 2 bytes 	number of sectors to be read, (some Phoenix BIOSes are limited to a maximum of 127 sectors)
//	04h..07h 4 bytes 	segment:offset pointer to the memory buffer to which sectors will be transferred (note that x86 is little-endian: if declaring the segment and offset separately, the offset must be declared before the segment)
//	08h..0Fh 8 bytes 	absolute number of the start of the sectors to be read (1st sector of drive has number 0)

struct __attribute__ ((packed)) BiosDiskAddressPacket
{
	unsigned char size;
	unsigned char pad;
	unsigned short nSectors;
	unsigned short dataOffset;
	unsigned short dataSegment;
	unsigned long long int absoluteSectorOffset;
};

#define BIOSBLOCKDEVICE_BUFFERSECTORS 	8	// reserve 4 kB block for reading and writing sectors

struct BiosBlockDevice
{

	struct BlockDevice	blockDev;

	// BIOS extensions present?
	bool biosExtensionsPresent;

	// Drive geometry
	unsigned int cylinders;
	unsigned int headsPerCylinder;
	unsigned int sectorsPerHead;

	//	for extended BIOS
	unsigned int physCylinders;
	unsigned int physHeadsPerCylinder;
	unsigned int physSectorsPerHead;
	unsigned long long int absoluteSectors; 

	struct BiosDiskAddressPacket* biosDAP;	

	// BIOS drive number
	unsigned int driveNumber;

	// low-mem buffer for BIOS calls
	unsigned char* sectorBuffer;

};

struct BiosBlockDevice* devices_biosBlock_createBIOSBlockDevice(unsigned char driveNumber);
void			devices_biosBlock_destroyBIOSBlockDevice(struct BiosBlockDevice** dev);
