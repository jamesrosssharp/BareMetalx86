
#include "../../Mem/mem.h"

#include "partition.h"

static int readSectors(struct BlockDevice* dev, void* dest, unsigned long long int offset, unsigned int nsectors)
{
	struct PartitionBlockDevice* bdev = (struct PartitionBlockDevice*)dev;

	// bounds check offset and nsectors

	if (offset >= (bdev->endSector - bdev->startSector))
		return 0;

	if (offset + (unsigned long long int)nsectors >= bdev->endSector)
		nsectors = (bdev->endSector - offset);

	return bdev->childDevice->readSectors(bdev->childDevice, dest, offset + bdev->startSector, nsectors);

}	
	
static bool readSectorsAsynch(struct BlockDevice* dev, void* dest, unsigned long long int offset, unsigned int nsectors,
					BlockDeviceAsynchCallback callback, void* data)
{
	return false;	// not implemented
}
	


struct PartitionBlockDevice* devices_partition_createPartitionBlockDevice(struct BlockDevice* bdev,
									  unsigned int partitionNumber,
									  unsigned int partitionType,
									  unsigned long long int startSector,
									  unsigned long long int endSector)
{

	struct PartitionBlockDevice* dev = (struct PartitionBlockDevice*)kmalloc(sizeof(struct PartitionBlockDevice), MEMORYTYPE_HIMEM);

	if (dev == NULL)
		return NULL;
	
	devices_block_initBlockDevice((struct BlockDevice*)dev, bdev->bytesPerSector);

	dev->childDevice = bdev;

	dev->partitionNumber = partitionNumber;
	dev->partitionType   = partitionType;
	dev->startSector     = startSector;
	dev->endSector 	     = endSector;

	dev->thisDevice.readSectors = readSectors;
	dev->thisDevice.readSectorsAsynch = readSectorsAsynch;

	return dev;	

}


void	devices_partition_destroyPartitionBlockDevice(struct PartitionBlockDevice* dev)
{
	kfree(dev);
}	
