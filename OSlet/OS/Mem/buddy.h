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

int mem_buddy_requiredMemorySize(int maxBlockSize, int minBlockSize);
int mem_buddy_maxBuddyBlockSizeForMemoryRegion(int memSize, int minBlockSize);
bool mem_buddy_init(int maxBlockSize, int minBlockSize, void* memory);
void mem_buddy_debug(struct BuddyMemoryAllocator* buddy);
void*	mem_buddy_allocate(struct BuddyMemoryAllocator* buddy, unsigned int bytes);
void mem_buddy_free(struct BuddyMemoryAllocator* buddy, void* mem);

#ifdef __cplusplus
}
#endif
