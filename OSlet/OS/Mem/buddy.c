/*
	
	"Buddy" memory allocator

This memory allocator uses a BTree to keep track of 
memory blocks, which are always power of two in size. 

In addition to the BTree, space must be reserved for a 
free list of memory block entries, which store the info
about the block (such as pid, block size etc.)

Memory Layout:
			256k
| 4k | 4k | 8k | 16k | 32k |  64k  | 128k    |
  0                         0x10000  0x20000  0x40000

BTree structure:

		   0x20000 (256k)
		/                  \
	     0x10000 (128k)  0x30000 (128k)
	     /	       \	
        0x8000 (64k)   0x18000 (64k)
        /         \
     0x4000 (32k)  0xc0000 (32k)			
     /         \
  0x2000(16k)  0x6000
   /	     \						
 0x1000 (8k)  0x3000
  /   \
0x800 0xc00 (4k)


*/

#include "buddy.h"
#include "../Lib/klib.h"

// This might change. We may want Buddy memory pools to be page aligned, for easy mapping into
// process memory.
#define BUDDY_BLOCK_ALIGN 4

int mem_buddy_requiredMemorySize(int maxBlockSize, int minBlockSize)
{
	if (! IS_POWER_OF_TWO(maxBlockSize) || ! IS_POWER_OF_TWO(minBlockSize))
		return -1;

	if (maxBlockSize == 0 || minBlockSize == 0)
		return -1;

	// compute size memory blocks

	int memSize = maxBlockSize + 3*BUDDY_BLOCK_ALIGN;	// we pad 3 times

	// compute size of BTree

	int treeDepth = lib_math_log2(maxBlockSize) - lib_math_log2(minBlockSize) + 1;	

	memSize += lib_btree_requiredMemorySize(treeDepth); 		

	// compute size of block freelist

	int numBlocks = lib_btree_maxElementsRequired(treeDepth);

	memSize += mem_freeList_requiredMemorySize(sizeof(struct BuddyBlock), numBlocks);

	return memSize;

}

int mem_buddy_maxBuddyBlockSizeForMemoryRegion(int memSize, int minBlockSize)
{

	int requiredMem = 0;

	int blockSizeLog2 = lib_math_log2(minBlockSize);

	int i = 0;

	while (1)
	{
	   requiredMem = mem_buddy_requiredMemorySize(1 << blockSizeLog2, minBlockSize);

	   if (requiredMem == -1)
		return -1;

	   if (requiredMem > memSize)
		break;

           i ++;
	   blockSizeLog2 ++;
	}

	if (i == 0)
		return -1;

	return 1 << (blockSizeLog2 - 1);
}


// Init a Buddy memory allocator. Allocate mem_buddy_requiredMemorySize storage for holding buddy allocator.
bool mem_buddy_init(int maxBlockSize, int minBlockSize, void* memory)
{
	
	// Init the BuddyMemoryAllocator structure 	

	struct BuddyMemoryAllocator* buddy = (struct BuddyMemoryAllocator*)memory;

	buddy->maxBlockSize = maxBlockSize;
	buddy->minBlockSize = minBlockSize;
	
	int treeDepth = lib_math_log2(maxBlockSize) - lib_math_log2(minBlockSize) + 1;

	buddy->btreeMaxDepth = treeDepth;

	// Allocate the storage tree.

	uintptr_t btreeOffset = ALIGNTO((uintptr_t)memory + sizeof(struct BuddyMemoryAllocator), BUDDY_BLOCK_ALIGN);

	int btreeSize = lib_btree_requiredMemorySize(treeDepth);  

	buddy->storageTree = (struct BTree*)btreeOffset;

	if (! lib_btree_init((void *)btreeOffset, treeDepth))
	{
		DEBUG("Could not init BTree\n");
		return false;
	}

	// Allocate the block record freelist.

	uintptr_t freeListOffset = ALIGNTO(btreeOffset + btreeSize, BUDDY_BLOCK_ALIGN);

	int numberOfBlocks = lib_btree_maxElementsRequired(treeDepth);

	int freeListSize = mem_freeList_requiredMemorySize(sizeof(struct BuddyBlock), numberOfBlocks);

	if (! mem_freeList_init(sizeof(struct BuddyBlock), numberOfBlocks, (void*)freeListOffset))
	{
		DEBUG("Could not init freelist!\n");
		return false;
	}

	buddy->blockFreeList = (struct FreeList*) freeListOffset;

	// Allocate the memory for the free blocks

	uintptr_t blocksOffset = ALIGNTO(freeListOffset + freeListSize, BUDDY_BLOCK_ALIGN);		

	buddy->blocksBaseAddress = (void*)blocksOffset;

	// Create BTree root

	struct BuddyBlock* block = mem_freeList_allocateBlock(buddy->blockFreeList);

	if (block == NULL)
	{
		DEBUG("Could not allocate block from freelist!\n");
		return false;
	}

	block->allocated = false;
	block->pid = 0;
	block->size = maxBlockSize;		
	block->baseAddress = (void*)blocksOffset;  

	if (! lib_btree_addElement(buddy->storageTree, maxBlockSize >> 1, (void*) block))
	{
		DEBUG("Could not add element to BTree!\n");
		return false;
	}

	return true;
		
}

void*	mem_buddy_allocate(struct BuddyMemoryAllocator* buddy, unsigned int bytes)
{

	// Find a leaf of the storage tree which has the smallest unallocated block bigger than the required size

		

}

void mem_buddy_printNodes(void* data, void* nodeData, int depth)
{

	for (int i = 0; i < depth; i ++) DEBUG(" "); 

	struct BuddyBlock* block = (struct BuddyBlock*)nodeData;

	DEBUG("Block: size: %d base address: %p allocated: %s\n", block->size, block->baseAddress, block->allocated ? "true" : "false");

}

void mem_buddy_debug(struct BuddyMemoryAllocator* buddy)
{
	DEBUG("============= Buddy Memory Allocator ===============\n");
	DEBUG("MaxBlockSize: %d MinBlockSize: %d Btree max depth: %d\n", buddy->maxBlockSize, buddy->minBlockSize, buddy->btreeMaxDepth);
	DEBUG("BTree: %p\n", buddy->storageTree);
	DEBUG("FreeList: %p\n", buddy->blockFreeList);
	DEBUG("Block Start: %p\n", buddy->blocksBaseAddress);

	//lib_btree_debugTree(buddy->storageTree);

	lib_btree_traverseTreeWithCallback(buddy->storageTree, false, mem_buddy_printNodes, NULL);
}
