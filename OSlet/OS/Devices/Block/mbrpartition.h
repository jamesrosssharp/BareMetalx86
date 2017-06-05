#pragma once

#include "partition.h"

void devices_mbrPartition_createPartitionDevices(struct BlockDevice* blockDevice, struct PartitionBlockDevice*** partitionDeviceList, 
						 unsigned int* nPartitions);


