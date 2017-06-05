#pragma once

#include "../Devices/Block/block.h"
#include "fs.h"

enum FATFileSystemType 
{
	FATFS_FAT12,
	FATFS_FAT16,
	FATFS_FAT32
};

struct FATFile
{
	struct File file;
	
	unsigned int startCluster;

};

struct FATFileSystem
{
	struct FileSystem fs;

	enum FATFileSystemType fsType;

	char OEMName[9];
	char pad[3];
	char volumeLabel[12];	

	unsigned int numFATs;
	unsigned int FATSize;

	unsigned int reservedSectors;

	unsigned long long int rootSector;	// sector start of cluster containing root directory
	unsigned int rootDirNumSectors;		// number of sectors in root directory
	unsigned int rootCluster;

	unsigned long long int fatSector;	// sector containing the FAT

	unsigned int bytesPerSector;
	unsigned int sectorsPerCluster;

	unsigned int deviceClusterBytes;
	unsigned char* clusterBuffer;

	struct UnicodeString* dirItemString;

	unsigned char* FATs;	// space for all FATs is allocated contiguously

};

struct FATFileSystem* fs_fat_createFATFileSystem(struct BlockDevice* bdev);
void fs_fat_destroyFATFileSystem(struct FATFileSystem** fs);
