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

// This might change. We may want Buddy memory pools to be page aligned, for easy mapping into
// process memory.
#define BUDDY_BLOCK_ALIGN 4

int mem_buddy_requiredMemorySize(int maxBlockSize, int minBlockSize)
{
	if (! IS_POWER_OF_TWO(blockSize) || ! IS_POWER_OF_TWO(minBlockSize))
		return -1;

	if (maxBlockSize == 0 || minBlockSize == 0)
		return -1;

	// compute size memory blocks

	int memSize = maxBlockSize + BUDDY_BLOCK_ALIGN;

	// compute size of BTree

	int treeDepth = lib_math_log2(maxBlockSize) - lib_math_log2(minBlockSize) + 1;	

	memSize += lib_btree_requiredMemorySize(treeDepth); 		

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

	   if (requiredMem > memSize)
		break;

           i ++;
	   blockSizeLog2 ++;
	}

	if (i == 0)
		return -1;

	return 1 << blockSizeLog2;
}


