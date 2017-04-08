/*
	
	"Buddy" memory allocator

This memory allocator uses a BTree to keep track of 
memory blocks, which are always power of two in size. 

In addition to the BTree, space must be reserved for a 
free list of memory block entries, which store the info
about the block (such as pid, block size etc.)

Sample Memory Layout:
			256k
| 4k | 4k | 8k | 16k | 32k |  64k  | 128k    |
  0                         0x10000  0x20000  0x40000

corresponding BTree structure:

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
0x800 0x1800 (4k)


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
	block->bisector = maxBlockSize >> 1; 

	if (! lib_btree_addElement(buddy->storageTree, maxBlockSize >> 1, (void*) block))
	{
		DEBUG("Could not add element to BTree!\n");
		return false;
	}

	return true;
		
}


void mem_buddy_findCompatibleBlock(struct BTreeNode* node, void* data, int depth)
{

	struct BuddyBlock* block = (struct BuddyBlock*)node->data;
	struct BuddySearchInfo* info = (struct BuddySearchInfo*)data;

	if (info == NULL || block == NULL)
		return;

	if ((! block->allocated) && (block->size < info->minBytesFound) && (block->size >= info->requestedBytes))
	{
		info->minBytesFound = block->size;
		info->minBytesDepth = depth;
		info->minBlock = block;	
		info->minBisector = node->bisector;
	}

}

void*	mem_buddy_allocate(struct BuddyMemoryAllocator* buddy, unsigned int bytes)
{

	DEBUG("Allocating %d bytes...\n", bytes);

	struct BuddySearchInfo info = {0};

	info.requestedBytes = bytes;
	info.minBytesFound = 0xffffffff;
	info.minBlock = NULL;

	// Find a leaf of the storage tree which has the smallest unallocated block bigger than the required size

	lib_btree_traverseTreeWithCallback(buddy->storageTree, true, mem_buddy_findCompatibleBlock, &info); 		

	if (info.minBlock == NULL)
		return NULL;

	int log2bytes = lib_math_log2(bytes);
	int log2found = lib_math_log2(info.minBytesFound);

	DEBUG("log2bytes: %d log2found: %d\n", log2bytes, log2found);

	if (log2bytes == log2found)
	{
		// The requested block can be contained by the block found, without the need
		// to sub divide.

		if (info.minBlock == NULL)
			return NULL;

		info.minBlock->allocated = true;

		DEBUG("Allocated %d byte block\n", info.minBlock->size);

		return info.minBlock->baseAddress;

	}
	else if (log2bytes < log2found)
	{
		DEBUG("Subdividing %d byte block\n", info.minBlock->size);

		int log2blockBytes = log2found;

		struct BuddyBlock* block = info.minBlock;
		int	bisector = info.minBisector;


		while ((log2blockBytes > log2bytes) && (log2blockBytes > lib_math_log2(buddy->minBlockSize)))
		{
			block->allocated = true;

			struct BuddyBlock* left = (struct BuddyBlock*)mem_freeList_allocateBlock((void*)buddy->blockFreeList);

		    	if (left == NULL)
			{
				DEBUG("Could not allocate left child node.\n");
				return NULL;
			}

			struct BuddyBlock* right = (struct BuddyBlock*)mem_freeList_allocateBlock((void*)buddy->blockFreeList);   	

			if (right == NULL)
			{
				DEBUG("Could not allocate right child node.\n");
				mem_freeList_freeBlock((void*)buddy->blockFreeList, (void*)left);
				return NULL;
			}
	
			// add children

			int leftBisector = bisector - (block->size >> 2);
			int rightBisector = bisector + (block->size >> 2);

			left->bisector = leftBisector;
			right->bisector = rightBisector;

			left->allocated = right->allocated = false;

			DEBUG("Left bisector: %x right: %x\n", leftBisector, rightBisector);

			right->size = left->size = block->size >> 1;

			DEBUG("Left size: %x right: %x\n", left->size, right->size);

			left->baseAddress = (void*)((uintptr_t)block->baseAddress);
			right->baseAddress = (void*)((uintptr_t)block->baseAddress + (block->size >> 1));

			DEBUG("Left address: %p right: %p\n", left->baseAddress, right->baseAddress);

			lib_btree_addElement(buddy->storageTree, leftBisector, left);
			lib_btree_addElement(buddy->storageTree, rightBisector, right);

			block = left;
			bisector = leftBisector;

			log2blockBytes --;
		}

		block->allocated = true;

		DEBUG("Allocated %d byte block\n", block->size);

		return block->baseAddress;

	
	}
	else
	{
		DEBUG("requested size greater than block size! r: %d a: %d\n", bytes, info.minBlock->size);
	}

	return NULL;

}


void mem_buddy_findBlockWithAddress(struct BTreeNode* node, void* data, int depth)
{

	struct BuddySearchBlockInfo* info = (struct BuddySearchBlockInfo*)data;

	struct BuddyBlock* block = (struct BuddyBlock*)node->data;

	if (block->baseAddress == info->requestedBaseAddress)
	{
		info->block = block;
		info->btreeNode = node;
	}

}

void mem_buddy_free(struct BuddyMemoryAllocator* buddy, void* mem)
{

	struct BuddySearchBlockInfo info;

	info.requestedBaseAddress = mem;
	info.block = NULL;	
	info.btreeNode = NULL;

	// Find the block with the baseAddress = mem

	lib_btree_traverseTreeWithCallback(buddy->storageTree, true, mem_buddy_findBlockWithAddress, &info); 		
	
	if (info.block == NULL || info.btreeNode == NULL)
	{
		DEBUG("Could not free block: %p\n", mem);

		return;
	}

	DEBUG("Found block for free: %p %p\n", info.block, info.btreeNode);	

	// Deallocate this block.

	info.block->allocated = false;

	// See if we can coalesce blocks

	struct BTreeNode* node = lib_btree_getParent(info.btreeNode);

	while (node != NULL)
	{

		struct BuddyBlock* block = (struct BuddyBlock*)lib_btree_getNodeData(node);

		if (block == NULL)
			break;
	
		// Check left and right nodes to see if they are allocated


		struct BuddyBlock* leftBlock = (struct BuddyBlock*)lib_btree_getLeftNodeData(node);
		struct BuddyBlock* rightBlock = (struct BuddyBlock*)lib_btree_getRightNodeData(node);

		if (rightBlock->allocated || leftBlock->allocated)
			break;

		// Children are free? Deallocate their blocks, and remove the node from the tree			

		DEBUG("Coalescing blocks: %p (%x) %p (%x)\n", leftBlock, leftBlock->bisector, rightBlock, rightBlock->bisector);

		if (rightBlock != NULL)
			mem_freeList_freeBlock(buddy->blockFreeList, rightBlock);
		
		if (leftBlock != NULL)
			mem_freeList_freeBlock(buddy->blockFreeList, leftBlock);

		lib_btree_makeNodeALeaf(buddy->storageTree, node);

		block->allocated = false;	

		node = lib_btree_getParent(node);

	}

}


void mem_buddy_printNodes(struct BTreeNode* node, void* data, int depth)
{

	for (int i = 0; i < depth; i ++) DEBUG(" "); 

	struct BuddyBlock* block = (struct BuddyBlock*)node->data;

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
