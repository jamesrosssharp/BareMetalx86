#pragma once

#include "block.h"

struct PartitionBlockDevice
{
	struct BlockDevice thisDevice;

	unsigned long long int startSector;
	unsigned long long int endSector;

	unsigned int partitionType;	

	unsigned int partitionNumber;

	struct BlockDevice* childDevice;
};

struct PartitionBlockDevice* devices_partition_createPartitionBlockDevice(struct BlockDevice* bdev,
									  unsigned int partitionNumber,
									  unsigned int partitionType,
									  unsigned long long int startSector,
									  unsigned long long int endSector);


void	devices_partition_destroyPartitionBlockDevice(struct PartitionBlockDevice* dev);		
