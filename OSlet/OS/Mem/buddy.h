#pragma once

#include "mem.h"

struct BuddyBlock
{

	bool		allocated;
	unsigned int	pid;
	unsigned int 	size;
	void*		baseAddress;	

};

struct BuddySearchInfo
{
	unsigned int requestedBytes;
	unsigned int minBytesFound;
	unsigned int minBytesDepth;
	struct BuddyBlock* minBlock;	
};

struct BuddyMemoryAllocator
{
	unsigned int maxBlockSize;
	unsigned int minBlockSize;

	unsigned int btreeMaxDepth;

	struct BTree* storageTree;

	struct FreeList* blockFreeList;

	void*	blocksBaseAddress;		

};


#ifdef __cplusplus
extern "C" {
#endif

int mem_buddy_requiredMemorySize(int maxBlockSize, int minBlockSize);
int mem_buddy_maxBuddyBlockSizeForMemoryRegion(int memSize, int minBlockSize);
bool mem_buddy_init(int maxBlockSize, int minBlockSize, void* memory);
void mem_buddy_debug(struct BuddyMemoryAllocator* buddy);

#ifdef __cplusplus
}
#endif
