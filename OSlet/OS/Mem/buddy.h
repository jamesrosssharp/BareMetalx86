#pragma once

#include "mem.h"
#include "../Lib/klib.h"

struct BuddyBlock
{

	bool		allocated;
	unsigned int	pid;
	unsigned int 	size;
	int		bisector;
	void*		baseAddress;	

};

struct BuddySearchInfo
{
	unsigned int requestedBytes;
	unsigned int minBytesFound;
	unsigned int minBytesDepth;
	int 	     minBisector;
	struct BuddyBlock* minBlock;	
};

struct BuddySearchBlockInfo
{
	void* requestedBaseAddress;
	struct BuddyBlock* block;
	struct BTreeNode* btreeNode; 
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

unsigned int mem_buddy_requiredMemorySize(unsigned int maxBlockSize, unsigned int minBlockSize);
unsigned int mem_buddy_requiredMemorySizeForAllocatorStructures(unsigned int maxBlockSize, unsigned int minBlockSize);

unsigned int mem_buddy_maxBuddyBlockSizeForMemoryRegion(unsigned int memSize, unsigned int minBlockSize);

bool mem_buddy_init(int maxBlockSize, int minBlockSize, void* memoryForStructures, void* memoryForAllocation);

void mem_buddy_debug(struct BuddyMemoryAllocator* buddy);
void*	mem_buddy_allocate(struct BuddyMemoryAllocator* buddy, unsigned int* bytes);
void mem_buddy_free(struct BuddyMemoryAllocator* buddy, void* mem);
int mem_buddy_estimateNumberOfBuddyAllocatorsForRegion(int size, int minBlockSize);

unsigned int mem_buddy_createBuddyMemoryPool(struct MemoryPool* pool, uintptr_t baseAddress, unsigned int size, unsigned int minBlockSize);

#ifdef __cplusplus
}
#endif
