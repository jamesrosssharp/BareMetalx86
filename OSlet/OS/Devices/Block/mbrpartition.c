
#include "mbrpartition.h"

#include "../../Console/console.h"
#include "../../Mem/mem.h"

void devices_mbrPartition_createPartitionDevices(struct BlockDevice* blockDevice, 
						 struct PartitionBlockDevice*** partitionDeviceList, 
						 unsigned int* nPartitions)
{

	if (partitionDeviceList == NULL || nPartitions == NULL)
		return;

	unsigned char buf[4096];

	if (blockDevice->bytesPerSector > 4096)
	{
		*partitionDeviceList = NULL;
		*nPartitions = 0;	
		return;
	}

	unsigned int sectors = blockDevice->readSectors(blockDevice, buf, 0, 1);  

	if (sectors != 1)
	{
		*partitionDeviceList = NULL;
		*nPartitions = 0;	
		return;	
	}

	unsigned short magic = *(unsigned short*)&buf[blockDevice->bytesPerSector - 2];

	if (magic != 0xaa55)
	{
		kprintf("Not MBR partition: %04x\n", magic);
		*partitionDeviceList = NULL;
		*nPartitions = 0;	
		return;	
	}

	unsigned char* MBR = &buf[0x1be];

	struct PartitionBlockDevice* partitions[4] = {NULL};

	*nPartitions = 0;

	for (int i = 0; i < 4; i ++)
	{

		// TODO: Use a struct for this...

		unsigned int fsType = MBR[4];
		unsigned int cs	    = MBR[2];
		unsigned int c	    = MBR[3];
		unsigned int h	    = MBR[1];

		unsigned int lba    = *(unsigned int*)&MBR[8];

		unsigned int len    = *(unsigned int*)&MBR[12];

		unsigned int lba_end = lba + len; // 1 sector size??

		if (fsType)
		{

			//kprintf("MBR %d: FS=%x Start C: %d H: %d S: %d LBA: %08x\n", 
			//		i,
			//		fsType,
			//		c | (cs & 0xc0)<<2,
			//		h,
			//		cs & 0x3f,
			//		lba);

			// create the partition

			partitions[*nPartitions] = devices_partition_createPartitionBlockDevice(blockDevice,
									  i,
									  fsType,
									  lba,
									  lba_end); 
			
			*nPartitions = *nPartitions + 1;

		}
			
		MBR += 16;
	}

	// Create the partitions structure to return


	*partitionDeviceList = (struct PartitionBlockDevice**)kmalloc(sizeof(struct PartitionBlockDevice*) * (*nPartitions), MEMORYTYPE_HIMEM);

	if (*partitionDeviceList == NULL)
	{

		for (int i = 0; i < *nPartitions; i++)
			// delete partition block device	
			devices_partition_destroyPartitionBlockDevice((*partitionDeviceList)[i]);		

		*nPartitions = 0;

		return;
	}

	
	for (int i = 0; i < *nPartitions; i++)
		(*partitionDeviceList)[i] = partitions[i];

}

