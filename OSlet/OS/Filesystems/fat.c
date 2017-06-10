#include "fat.h"
#include "../Mem/mem.h"
#include "../Console/console.h"
#include "../Lib/klib.h"

struct __attribute__((packed)) FATBPBBase
{
	char		jumpBoot[3]; 
	char		FAT_BPB_OEMName[8];
	unsigned short  FAT_BPB_BytesPerSec;
	unsigned char   FAT_BPB_SecPerClus;
	unsigned short  FAT_BPB_ResvdSecCnt;
	unsigned char   FAT_BPB_NumFATs;
	unsigned short  FAT_BPB_RootEntCnt;
	unsigned short  FAT_BPB_TotSec16;
	unsigned char 	FAT_BPB_Media; 		
	unsigned short	FAT_BPB_FATSz16;
	unsigned short	FAT_BPB_SecPerTrk;
	unsigned short  FAT_BPB_NumHeads;
	unsigned int	FAT_BPB_HiddSec;
	unsigned int	FAT_BPB_TotSec32;

};

struct __attribute__((packed)) FAT1216BPB
{
	struct FATBPBBase base;
	
	unsigned char	FAT_BS_DrvNum;
	unsigned char	FAT_BS_Reserved1;
	unsigned char	FAT_BS_BootSig;
	unsigned int	FAT_BS_VolID;
	unsigned char	FAT_BS_VolLab[11];
	unsigned char	FAT_BS_FilSysType[8];

};

struct __attribute__((packed)) FAT32BPB
{
	struct FATBPBBase base;

	unsigned int	FAT32_BPB_FATSz32; 
	unsigned short	FAT32_BPB_ExtFlags; 
	unsigned short	FAT32_BPB_FSVer;
	unsigned int	FAT32_BPB_RootClus;
	unsigned short	FAT32_BPB_FSInfo; 
	unsigned short	FAT32_BPB_BkBootSec; 
	unsigned char	FAT32_BPB_Reserved[12]; 

	unsigned char	FAT32_BS_DrvNum;
	unsigned char	FAT32_BS_Reserved1;
	unsigned char	FAT32_BS_BootSig;
	unsigned int	FAT32_BS_VolID;
	unsigned char	FAT32_BS_VolLab[11];
	unsigned char	FAT32_BS_FilSysType; 
};

struct __attribute__((packed)) FATDirectory
{
	char DIR_Name[11];
	unsigned char DIR_Attr;
	unsigned char DIR_NTRes;
	unsigned char DIR_CrtTimeTenth;
	unsigned short DIR_CrtTime;
	unsigned short DIR_CrtDate;
	unsigned short DIR_LstAccDate; 
	unsigned short DIR_FstClusHI; 
	unsigned short DIR_WrtTime;
	unsigned short DIR_WrtDate;
	unsigned short DIR_FstClusLO;
	unsigned int   DIR_FileSize;
};


struct __attribute__((packed)) FATLongDirEntry
{
	unsigned char  LDIR_Ord;
	unsigned short LDIR_Name1[5];
	unsigned char  LDIR_Attr;
	unsigned char  LDIR_Type;
	unsigned char  LDIR_Chksum;
	unsigned short LDIR_Name2[6];
	unsigned short LDIR_FstClusLO;
	unsigned short LDIR_Name3[2];
};

struct FATFindFileDirEntry
{
	unsigned int cluster;
	struct FATDirectory dirItem;	
};

#define FINDFILE_INVALIDCLUSTER 0xffffffff

#define FAT_DIR_ATTR_READ_ONLY	0x01
#define FAT_DIR_ATTR_HIDDEN	0x02
#define FAT_DIR_ATTR_SYSTEM	0x04
#define FAT_DIR_ATTR_VOLUME_ID	0x08
#define FAT_DIR_ATTR_DIRECTORY	0x10
#define FAT_DIR_ATTR_ARCHIVE	0x20
#define FAT_DIR_ATTR_LONG_NAME	(FAT_DIR_ATTR_READ_ONLY | FAT_DIR_ATTR_HIDDEN | FAT_DIR_ATTR_SYSTEM | FAT_DIR_ATTR_VOLUME_ID)
#define FAT_DIR_ATTR_MASK	0x3f

#define FAT_NUM_CHARS_PER_LONG_DIR 13
#define FAT_LONG_DIR_MAX_CHARS	260

static void printLongName(unsigned short* longName)
{

	unsigned short val;
	char buf[260];

	char* bufPtr = buf;

	do
	{
		val = *longName++;

		if (val)
			*bufPtr++ = val & 0x7f;			

	} while (val);	

	*bufPtr = '\0';

	kprintf("%s", buf);

}

static unsigned char calcCheckSum(struct FATDirectory* dir)
{

	unsigned char sum = 0;

	unsigned char* str = (unsigned char*)&dir->DIR_Name;

	for (int i = 0; i < 11; i ++)
	{
		sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *str++; // microsoft's checksum routine
	}	

	return sum;

}

static bool findLongFileName(struct FATDirectory* dir, struct FATDirectory* headDir, unsigned short* longName, unsigned char sum)
{

	while (dir > headDir)
	{
		dir--;

		// Is it a long name entry? If not, return false (long name entries must be physically contiguous)

		if ((dir->DIR_Attr & FAT_DIR_ATTR_MASK) != FAT_DIR_ATTR_LONG_NAME)
			return false;
		else
		{

			struct FATLongDirEntry* longDir = (struct FATLongDirEntry*)dir;

			// Does it match the checksum? If not, return false (found an orphan)
			if (longDir->LDIR_Chksum != sum)
			{	
				kprintf("Found orphan! %02x %02x\n", longDir->LDIR_Chksum, sum);
				return false;
			}
			else
			{

				unsigned char ord = longDir->LDIR_Ord;

				unsigned int idx = 0;

				idx = (ord & 0x3f) - 1;
				
				// copy file name portion

				unsigned int offset = idx * FAT_NUM_CHARS_PER_LONG_DIR;	

				if (offset + FAT_NUM_CHARS_PER_LONG_DIR < FAT_LONG_DIR_MAX_CHARS)
				{
					lib_memcpy(&longName[offset], longDir->LDIR_Name1, 5 * sizeof(unsigned short));
					lib_memcpy(&longName[offset + 5 ], longDir->LDIR_Name2, 6 * sizeof(unsigned short));
					lib_memcpy(&longName[offset + 11], longDir->LDIR_Name3, 2 * sizeof(unsigned short));
					
				}

				// What is the ordinal? If bit 10 is set, we have found the final long entry. Return true

				if (ord & 0x40)
					return true;
			
			}

		}

	}

	return false;	// we didn't find the head long entry

}

static struct FATFindFileDirEntry parseDirectoryForEntry(struct FATFileSystem* fs, void* buffer, unsigned int size, struct UnicodeString* entry)	
{

	struct FATDirectory* dir = (struct FATDirectory*)buffer;

	unsigned short longName[FAT_LONG_DIR_MAX_CHARS];
	unsigned int longNameLength; 

	bool parseLongDirectoryList = false;

	// Find possible short name from given path.
	// If the given path fits DOS8.3 format, create a 
	// short name string that can be compared with short directory entries.

	unsigned int start = 0, end = entry->stringLength, startBase, endBase, startExt, endExt;

	entry->getSubstringRange(entry, &start, &end);

	startBase = start;

	entry->tokenise(entry, '.', &startBase, &endBase);

	startExt = endBase;

	entry->tokenise(entry, '.', &startExt, &endExt);
					
	char dirName[12];
	char baseName[10];
	char extension[10];  
	char pathName8_3[12];				

	bool gotBaseName = false;
	bool gotExt = false;

	for (int i = 0; i < 11; i ++)
		pathName8_3[i] = ' ';

	pathName8_3[11] = '\0';

	entry->setSubstringRange(entry, startBase, endBase);

	gotBaseName = entry->toASCIICString(entry, baseName, sizeof(baseName));

	entry->setSubstringRange(entry, startExt, endExt);

	gotExt = entry->toASCIICString(entry, extension, sizeof(extension));

	if (gotBaseName)
	{
		// clear pathName to all spaces

			// copy base name

		char* p = baseName;
		int i = 0;
		while (*p)
			pathName8_3[i++] = *p++;	

		// copy extension
		p = extension;
		i = 8;
		while (*p)
			pathName8_3[i++] = *p++;	

	}

	entry->setSubstringRange(entry, start, end);

	// convert pathName8_3 to upper case

	lib_strtoupper(pathName8_3);

	for (int i = 0; i < size; i ++)
	{


		// If the first character of the directory name is zero, then all
		// other directory entries will be zero.
		if (dir->DIR_Name[0] == 0)
			break;

		if ((dir->DIR_Attr & FAT_DIR_ATTR_MASK) != FAT_DIR_ATTR_LONG_NAME)
		{

			// compute "checksum"

			unsigned char sum = calcCheckSum(dir);

			// find all possible long directory entries for this "checksum"
			// long entries are always immediately preceding and physically contiguous,
			// so they can't span clusters.

			lib_bzero(longName, FAT_LONG_DIR_MAX_CHARS);

			if (findLongFileName(dir, (struct FATDirectory*) buffer, longName, sum))
			{
				// File has a long name. Try to match to entry

				fs->dirItemString->reset(fs->dirItemString);

				longNameLength = 0;

				while (longName[longNameLength++])	
					if (longNameLength >= FAT_LONG_DIR_MAX_CHARS)
						break;

				// length includes trailing zero, so decrement by one
				if (longNameLength)
					longNameLength --;

				fs->dirItemString->appendUTF16String(fs->dirItemString, longName, longNameLength);

				kprintf("Found long dir item: ");
				fs->dirItemString->print(fs->dirItemString);
				kprintf("\n");

				if (entry->compare(entry, fs->dirItemString) == 0)
				{
					kprintf("Found matching directory item!\n");

					// return cluster of directory

					return (struct FATFindFileDirEntry) {dir->DIR_FstClusLO | (dir->DIR_FstClusHI << 16), *dir};
				}

			}
			else
			{
	
				// string compare

				if (lib_strncmp(dir->DIR_Name, pathName8_3, 11) == 0)
				{
					kprintf("Found matching directory (short) item! %s %s\n", dir->DIR_Name, pathName8_3);

					// return cluster of directory

					return (struct FATFindFileDirEntry) {dir->DIR_FstClusLO | (dir->DIR_FstClusHI << 16), *dir};
					
				}
				
			}

			

		}


		dir ++;
	}

	// return invalid directory cluster	

	return (struct FATFindFileDirEntry) {FINDFILE_INVALIDCLUSTER, {}};	

}

static unsigned long long int sectorForCluster(struct FATFileSystem* fs, unsigned int cluster)
{

	unsigned long long int firstDataSector = fs->numFATs*fs->FATSize + fs->reservedSectors + fs->rootDirNumSectors;	

	return (cluster - 2)*fs->sectorsPerCluster + firstDataSector;

}

static void getNextPathItem(struct UnicodeString* path, unsigned int* start, unsigned int* end, bool* inThisDir)
{

	unsigned int nextStart = 0, nextEnd;

	*end = *start = 0;

	path->getSubstringRange(path, start, end);

	nextStart = *end;
	nextEnd = path->stringLength;

	if (path->tokenise(path, '/', &nextStart, &nextEnd))	
	{
		
		if (nextEnd >= path->stringLength - 1)
			*inThisDir = true;

		path->setSubstringRange(path, nextStart, nextEnd); 
	}

}

static struct FATFindFileDirEntry findFileRecursive(struct FATFileSystem* fs, unsigned int dirCluster, struct UnicodeString* path, bool tokenisePath)
{

	// read in a cluster worth of bytes into the cluster buffer, starting from the dirCluster cluster offset

	// Check sector size.
	// TODO: make this support different dize sectors between block device and FS.
	if (fs->bytesPerSector != fs->fs.bdev->bytesPerSector)
		return (struct FATFindFileDirEntry) {FINDFILE_INVALIDCLUSTER, {}};

	if (fs->fs.bdev->readSectors(fs->fs.bdev, fs->clusterBuffer, sectorForCluster(fs, dirCluster), fs->sectorsPerCluster) 
					!= fs->sectorsPerCluster)
		return (struct FATFindFileDirEntry) {FINDFILE_INVALIDCLUSTER, {}};

	// tokenize the path, and get the first tokenized item, to find in this directory.

	bool itemThisLevel = false;

	unsigned int start = 0, end;

	getNextPathItem(path, &start, &end, &itemThisLevel);
	
	kprintf("Searching (recursive) for path item:");
	path->print(path);
	kprintf("\n");

	// enumerate directory entries in this directory

	struct FATFindFileDirEntry dir = parseDirectoryForEntry(fs, fs->clusterBuffer, 
						      (fs->rootDirNumSectors * fs->bytesPerSector / sizeof(struct FATDirectory)), 
						      path);	


	// path matches? recurse
	
	if (dir.cluster != FINDFILE_INVALIDCLUSTER)
	{
		if (itemThisLevel)
			return dir;
		else
			return findFileRecursive(fs, dir.cluster, path, true);	// this should be the path minus the first subdirectory, if any
	}
	else // did not find a match - try to get next cluster in cluster chain
	{
		kprintf("Multi-cluster directories not yet supported!\n");
	}

	return (struct FATFindFileDirEntry) {FINDFILE_INVALIDCLUSTER, {}};

}

static struct FATFindFileDirEntry findFile(struct FATFileSystem* fs, struct UnicodeString* path)
{

	kprintf("Finding: ");
	path->print(path);
	kprintf("\n");
	
	// For FAT32, recurse over cluster chain (root directory is like any other directory)

	if (fs->fsType == FATFS_FAT32)
	{
		return findFileRecursive(fs, fs->rootCluster, path, true);
	}

	// For FAT12 and FAT16, read in root directory, which is fixed size,
	// and look for first part of path, to obtain a cluster for the next directory,
	// from which we can recurse

	// Check sector size.
	// TODO: make this support different dize sectors between block device and FS.
	if (fs->bytesPerSector != fs->fs.bdev->bytesPerSector)
		return (struct FATFindFileDirEntry) {FINDFILE_INVALIDCLUSTER, {}};

	if (fs->fs.bdev->readSectors(fs->fs.bdev, fs->clusterBuffer, fs->rootSector, fs->rootDirNumSectors) != fs->rootDirNumSectors)
		return (struct FATFindFileDirEntry) {FINDFILE_INVALIDCLUSTER, {}};

	// Tokenise the path. Is the file in the root directory?

	unsigned int start = 0, end;
	bool isRootDirItem = false;

	getNextPathItem(path, &start, &end, &isRootDirItem);

	kprintf("Searching for path item:");
	path->print(path);
	kprintf("\n");

	// file (potentially) in the root directory. Find the cluster of the file.
	

	// file in a subdirectory. Find the subdirectory's cluster and recurse.

	struct FATFindFileDirEntry dir = parseDirectoryForEntry(fs, fs->clusterBuffer, 
						      (fs->rootDirNumSectors * fs->bytesPerSector / sizeof(struct FATDirectory)), 
						      path);	


	if (dir.cluster != FINDFILE_INVALIDCLUSTER)
	{
		if (isRootDirItem)
			return dir;

		return findFileRecursive(fs, dir.cluster, path, true);	// this should be the path minus the first subdirectory, if any
	}

	return (struct FATFindFileDirEntry) {FINDFILE_INVALIDCLUSTER, {}};

}

unsigned int getNextFATCluster(struct FATFileSystem* fs, unsigned int cluster)
{

	unsigned int fatEntry;

	switch (fs->fsType)
	{
		case FATFS_FAT12:

			kprintf("FAT12 Not yet supported.\n");	
			return 0xff8;

			break;
		case FATFS_FAT16:

			fatEntry = cluster << 1;
		
			return *(unsigned short*)&fs->FATs[fatEntry];

		case FATFS_FAT32:

			fatEntry = cluster << 2;
		
			return *(unsigned int*)&fs->FATs[fatEntry];

	}

}

unsigned int clusterIsEOC(struct FATFileSystem* fs, unsigned int nextCluster)
{
	switch (fs->fsType)
	{
		case FATFS_FAT12:

			if (nextCluster >= 0xff8)
				return true;
			
			break;
		case FATFS_FAT16:

			if (nextCluster >= 0xfff8)
				return true;
	
			break;
		case FATFS_FAT32:

			if (nextCluster >= 0x0ffffff8)
				return true;
	
			break;
	}


	return false;

}

static size_t read(struct File* file, void* buf, size_t count)
{

	struct FATFile* ff = (struct FATFile*)file;
	struct FATFileSystem* ffs = (struct FATFileSystem*)file->fs;

	// find which cluster offset is in

	if (ff->startCluster == FINDFILE_INVALIDCLUSTER)
		return 0;

	unsigned int cluster = ff->startCluster;
	unsigned int nextCluster = cluster;
	off_t	     clusterOffset = 0;

	unsigned int offsetIntoCluster = 0;
	
	while (1)
	{

		nextCluster = getNextFATCluster(ffs, cluster);

		offsetIntoCluster = file->offset - clusterOffset; 

		clusterOffset += ffs->bytesPerSector * ffs->sectorsPerCluster;

		if (clusterOffset >= file->offset)
		{

			break;
		}

		if (clusterIsEOC(ffs, nextCluster))
		{
			// TODO: error codes
			return 0;	 // can't find another cluster, and we haven't reached the offset: offset is past end of file
		}

		cluster = nextCluster;

	} 
 
	// while we have bytes remaining, read into the cluster buffer 
	// and copy into destination buffer
		
	unsigned int bytesRead = 0;

	while (1)
	{

		if (ffs->bytesPerSector != ffs->fs.bdev->bytesPerSector)
		{
			kprintf("Sector size mismatch! %d %d\n", ffs->bytesPerSector,  ffs->fs.bdev->bytesPerSector);
			return -1;
		}

		if (ffs->fs.bdev->readSectors(ffs->fs.bdev, ffs->clusterBuffer, sectorForCluster(ffs, cluster), ffs->sectorsPerCluster) 
					!= ffs->sectorsPerCluster)
			return bytesRead;

		unsigned int bytesInCluster = ffs->bytesPerSector * ffs->sectorsPerCluster - offsetIntoCluster;

		unsigned int bytesToCopy = bytesInCluster < count ? bytesInCluster : count;

		lib_memcpy(buf, ffs->clusterBuffer + offsetIntoCluster, bytesToCopy);

		bytesRead += bytesToCopy;

		offsetIntoCluster = 0;

		if (bytesRead >= count)
			break;


		nextCluster = getNextFATCluster(ffs, cluster);

		if (clusterIsEOC(ffs, nextCluster))
			break;

		cluster = nextCluster;

	}

	return bytesRead;

}

static off_t  lseek(struct File* file, off_t offset, int whence)
{

}

static void   close(struct File* file)
{


}

static struct File* open(struct FileSystem* fs, struct UnicodeString* path, int flags)
{

	if (fs == NULL)
		return NULL;

	struct FATFile* file = (struct FATFile*) kmalloc(sizeof(struct FATFile), MEMORYTYPE_HIMEM);

	if (file == NULL)
		return NULL;

	file->file.size = 0;
	file->file.offset = 0;
	file->startCluster = FINDFILE_INVALIDCLUSTER;

	// Recurse and find file. If the path doesn't exist, we may have to create the file.
	struct FATFindFileDirEntry dir = findFile((struct FATFileSystem*)fs, path);	

	kprintf("Item found at cluster %d\n", dir.cluster);


	switch (flags & O_ACCMODE)
	{
		case O_RDONLY:
		{

			if (dir.cluster == FINDFILE_INVALIDCLUSTER || (dir.dirItem.DIR_Attr & FAT_DIR_ATTR_DIRECTORY))
			{
				kfree(file);
				return NULL;
			}

			file->file.mode = FILEMODE_READABLE; 

			file->startCluster = dir.cluster;	
			file->file.size = dir.dirItem.DIR_FileSize;

			break;
		}
		case O_WRONLY:
		{
			// If FAT_DIR_ATTR_READONLY, error out.

			if ((dir.dirItem.DIR_Attr & FAT_DIR_ATTR_READ_ONLY) ||
			    (dir.dirItem.DIR_Attr & FAT_DIR_ATTR_DIRECTORY) || 
			    ((dir.cluster == FINDFILE_INVALIDCLUSTER) && ! (flags & O_CREAT)))
			{
				kfree(file);
				return NULL;
			}
	
			if (dir.cluster == FINDFILE_INVALIDCLUSTER)
			{
				// try to create the file

				kfree(file);
				return NULL;
			}
			else
			{
				file->startCluster = dir.cluster;	
				file->file.size = dir.dirItem.DIR_FileSize;
			}		

			file->file.mode = FILEMODE_WRITEABLE;

			
			break;
		}
		case O_RDWR:
		{
			// If FAT_DIR_ATTR_READONLY, error out.

			if ((dir.dirItem.DIR_Attr & FAT_DIR_ATTR_READ_ONLY) ||
			    (dir.dirItem.DIR_Attr & FAT_DIR_ATTR_DIRECTORY) || 
			    ((dir.cluster == FINDFILE_INVALIDCLUSTER) && ! (flags & O_CREAT)))
			{
				kfree(file);
				return NULL;
			}
		
			if (dir.cluster == FINDFILE_INVALIDCLUSTER)
			{
				// try to create the file

				kfree(file);
				return NULL;
			}
			else
			{
				file->startCluster = dir.cluster;	
				file->file.size = dir.dirItem.DIR_FileSize;
			}		
	
			file->file.mode = FILEMODE_READABLE | FILEMODE_WRITEABLE;	


			break;
		}

	} 

	// Fill in function pointers

	file->file.read = read;
	file->file.lseek = lseek;
	file->file.close = close;	

	file->file.fs = (struct FileSystem*)fs;

	return (struct File*)file;			

}

struct FATFileSystem* fs_fat_createFATFileSystem(struct BlockDevice* bdev)
{

	struct FATFileSystem* fs = (struct FATFileSystem*) kmalloc(sizeof(struct FATFileSystem), MEMORYTYPE_HIMEM); 

	if (fs == NULL)
		return NULL;

	if (! fs_initFileSystem((struct FileSystem*)fs, bdev))
	{
		kfree(fs);
		return NULL;
	}

	// Read the FAT BPB and determine FAT type

	if (bdev->bytesPerSector != 512)
	{
		kprintf("FAT Filesystem: Sector size is not 512 bytes!\n");
		kfree(fs);
		return NULL;
	}

	unsigned char data[512];
	
	if (bdev->readSectors(bdev, data, 0, 1) != 1)
	{
		kprintf("FAT Filesystem: could not read sectors from device!\n");
		kfree(fs);
		return NULL;
	}
	
	if (data[510] != 0x55 || data[511] != 0xaa)
	{
		kprintf("Not a FAT filesystem");
		kfree(fs);
		return NULL;
	}


	struct FATBPBBase* bpb 		= (struct FATBPBBase*) data;
	struct FAT1216BPB* fat1216bpb 	= (struct FAT1216BPB*)data;
	struct FAT32BPB* fat32bpb 	= (struct FAT32BPB*)data;
	
	// Try to compute FAT type

	// 1. RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;	

	unsigned int rootDirSectors = ((bpb->FAT_BPB_RootEntCnt << 5) + (bpb->FAT_BPB_BytesPerSec - 1)) / bpb->FAT_BPB_BytesPerSec;

	kprintf("rootDirSectors: %d\n", rootDirSectors);

	fs->rootDirNumSectors = rootDirSectors;

	unsigned int fatSize = (bpb->FAT_BPB_FATSz16 != 0) ? bpb->FAT_BPB_FATSz16 : fat32bpb->FAT32_BPB_FATSz32;

	unsigned int totalSectors = (bpb->FAT_BPB_TotSec16 != 0) ? bpb->FAT_BPB_TotSec16 : bpb->FAT_BPB_TotSec32;

	unsigned int dataSectors = totalSectors - (bpb->FAT_BPB_ResvdSecCnt + (bpb->FAT_BPB_NumFATs * fatSize) + rootDirSectors);

	kprintf("fatSize: %d totalSectors: %d dataSectors: %d\n", fatSize, totalSectors, dataSectors);

	// CountofClusters = DataSec / BPB_SecPerClus;

	unsigned int countOfClusters = dataSectors / bpb->FAT_BPB_SecPerClus;

	kprintf("Clusters: %d\n", countOfClusters);

	if (countOfClusters < 4085)
	{
		kprintf("FAT12 Partition\n");
		fs->fsType = FATFS_FAT12;	
	}
	else if (countOfClusters < 65525)
	{
		kprintf("FAT16 Partition\n");
		fs->fsType = FATFS_FAT16;	
	}
	else
	{
		kprintf("FAT32 Partition\n");
		fs->fsType = FATFS_FAT32;	
	}

	// Set up important members

	lib_memcpy(fs->OEMName, bpb->FAT_BPB_OEMName, 8);
	fs->OEMName[8] = '\0';
	
	switch(fs->fsType)
	{
		case FATFS_FAT12:
		case FATFS_FAT16:
			lib_memcpy(fs->volumeLabel, fat1216bpb->FAT_BS_VolLab, 11);			
			break;
		case FATFS_FAT32:
			lib_memcpy(fs->volumeLabel, fat32bpb->FAT32_BS_VolLab, 11);			
			break;
	}

	fs->volumeLabel[11] = '\0';

	kprintf("%s %s\n", fs->OEMName, fs->volumeLabel);

	fs->numFATs = bpb->FAT_BPB_NumFATs;

	fs->reservedSectors = bpb->FAT_BPB_ResvdSecCnt;

	fs->bytesPerSector = bpb->FAT_BPB_BytesPerSec;
	fs->sectorsPerCluster = bpb->FAT_BPB_SecPerClus;

	switch(fs->fsType)
	{
		case FATFS_FAT12:
		case FATFS_FAT16:
			fs->FATSize = bpb->FAT_BPB_FATSz16;	
			fs->rootSector = bpb->FAT_BPB_ResvdSecCnt + (fs->numFATs * bpb->FAT_BPB_FATSz16);		
			fs->rootCluster = 0;
			break;
		case FATFS_FAT32:
			fs->FATSize = fat32bpb->FAT32_BPB_FATSz32;
			fs->rootSector = sectorForCluster(fs, fat32bpb->FAT32_BPB_RootClus);
			fs->rootCluster = fat32bpb->FAT32_BPB_RootClus;
			break;
	}

	fs->fatSector = bpb->FAT_BPB_ResvdSecCnt;

	// fill in function pointers

	fs->fs.open = open;

	// allocate memory for directory item string

	fs->dirItemString = lib_createUnicodeString();

	if (fs->dirItemString == NULL)
	{
		kprintf("Could not allocate memory for dirItemString.\n");
		kfree(fs);
		return NULL;
	}	

	// allocate memory for FATs

	fs->FATs = (unsigned char*)kmalloc(fs->numFATs * fs->FATSize * fs->bytesPerSector, MEMORYTYPE_HIMEM);

	if (fs->FATs == NULL)
	{
		kprintf("Could not allocate memory for FATs.\n");
		lib_destroyUnicodeString(&fs->dirItemString);
		kfree(fs);
		return NULL;
	}	

	// read in FATs

	unsigned int FATSectors = fs->numFATs * fs->FATSize;

	if (fs->fs.bdev->readSectors(fs->fs.bdev, fs->FATs, fs->reservedSectors, FATSectors) != FATSectors)
	{
		kprintf("Could not read FAT.\n");
		lib_destroyUnicodeString(&fs->dirItemString);
		kfree(fs->FATs);
		kfree(fs);
		return NULL;
	}

	// allocate memory for cluster buffer

	unsigned int clusterBytes =  ((fs->bytesPerSector * fs->sectorsPerCluster + bdev->bytesPerSector - 1) / bdev->bytesPerSector) *
					bdev->bytesPerSector;

	unsigned int rootSectorBytes = fs->rootDirNumSectors * fs->bytesPerSector;

	if (rootSectorBytes > clusterBytes)
		clusterBytes = rootSectorBytes;


	fs->deviceClusterBytes = clusterBytes;

	fs->clusterBuffer = (unsigned char*)kmalloc(clusterBytes, MEMORYTYPE_HIMEM);

	if (fs->clusterBuffer == NULL)
	{
		kprintf("Could not allocate memory for cluster buffer.\n");
		lib_destroyUnicodeString(&fs->dirItemString);
		kfree(fs->FATs);
		kfree(fs);
		return NULL;
	}

	return fs;

}

void fs_fat_destroyFATFileSystem(struct FATFileSystem** fs)
{

	if (fs == NULL || *fs == NULL)
		return;
	
	lib_destroyUnicodeString(&(*fs)->dirItemString);
	kfree((*fs)->FATs);
	kfree((*fs)->clusterBuffer);
	kfree((*fs));

	*fs = NULL;
}
