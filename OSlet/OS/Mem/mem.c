
#include "mem.h"
#include "../Console/console.h"
#include "../BIOS/bios.h"

#define MEMORY_RESERVE_KERNEL		32*1024*1024	// 4MB kernel size
#define MEMORY_RESERVE_KERNEL_STACK	1*1024*1024	// 1MB kernel stack size

#define MEMORY_MINBLOCKSIZE		KERNEL_PAGE_SIZE	

#define MEMORY_POOLTYPE			MEMORYPOOLTYPE_BUDDY	

unsigned int mem_createMemoryPool(struct MemoryPool* pool, void* baseAddress, unsigned int size, enum MemoryPoolType type);
unsigned int mem_estimateNumberOfMemoryPools(enum MemoryPoolType type, unsigned int size);

struct MemoryPool *gMemoryPools;
int		   gNumMemoryPools = 0;

struct MemoryPool *gLowMemoryPools;
int		  gNumLowMemoryPools = 0;

bool	mem_initHimem()
{

	kprintf("Initing Hi-memory...");	

	// Query BIOS memory map

	struct MemoryEntry* memoryMap;

	int memMapSize = 0;
	struct MemoryEntry* memMap = 0; 

	if (! bios_detectMemory(&memMap, &memMapSize) || memMapSize == 0)
	{
		kprintf("Could not detect memory map...");
		return false;
	}

	int maxMemoryPools = 0;

	for (int i = 0; i < memMapSize; i ++)
	{
		// We want memory above 1 Meg.
		if ((memMap[i].baseAddress >= KERNEL_LOAD_ADDRESS) &&
		    (memMap[i].baseAddress <  0x100000000ULL) &&
  		    (memMap[i].type == MEMORY_TYPE_FREE) )
		{
			kprintf("Found usable memory at %08x%08x %d\n", 
				(unsigned int)(memMap[i].baseAddress >> 32), 
				(unsigned int)(memMap[i].baseAddress & 0xffffffffU), i);


			unsigned int addr = memMap[i].baseAddress & 0xffffffffU;
			unsigned int size = MIN(memMap[i].size, 0xffffffffU - addr);

			maxMemoryPools += mem_estimateNumberOfMemoryPools(MEMORY_POOLTYPE, size); 
		}		
	}
	
	kprintf("Maximum memory pools for usable memory regions: %d\n", maxMemoryPools);

	// find the memory at 0x100000, this is the kernel load address. Need to
	// reserve space for the kernel, it's stack, and the memory pools array.
	// (we reserve an array of maxMemoryPools in size of struct MemoryPool
	// for allocating and freeing memory.) 	

	int i;

	for (i = 0; i < memMapSize; i++)
	{
		if ((memMap[i].baseAddress <= KERNEL_LOAD_ADDRESS) &&
		    ((memMap[i].baseAddress + memMap[i].size) > KERNEL_LOAD_ADDRESS) &&
  		    (memMap[i].type == MEMORY_TYPE_FREE) )
		{
			break;		
		}

	}

	if (i >= memMapSize)
	{
		// panic and halt kernel
		kpanic("Could not find memory containing kernel load address. How is this kernel even executing?\n");
	}

	// We have the memory block containing the kernel load address.
	// Now reserve some memory, and partition the remainder into memory pools. 
			
	unsigned int address = KERNEL_LOAD_ADDRESS;	// this is a physical address

	unsigned int memSize = (unsigned int)(MIN(0xffffffffU - address, memMap[i].size - (address - memMap[i].baseAddress) ));

	address += MEMORY_RESERVE_KERNEL + MEMORY_RESERVE_KERNEL_STACK;

	memSize -= MEMORY_RESERVE_KERNEL + MEMORY_RESERVE_KERNEL_STACK;

	
	// To do paging: page in memory for memory pool structure.
	
	gMemoryPools = (struct MemoryPool*)address; // this should be a virtual address mapped into kernel page table
	
	int memPoolReserveBytes = ALIGNTO(sizeof(struct MemoryPool) * maxMemoryPools, MEMORY_MINBLOCKSIZE); 

	lib_bzero(gMemoryPools, memPoolReserveBytes);

	address += memPoolReserveBytes;

	memSize -= memPoolReserveBytes; 

	unsigned int freeAddress = address;

	unsigned int bytes = memSize;

	gNumMemoryPools = 0;

	// find minimum allocatable bytes

	unsigned int minAllocatableBytes = bytes;

	switch (MEMORY_POOLTYPE)
	{
		case MEMORYPOOLTYPE_BUDDY:
		{
			minAllocatableBytes = mem_buddy_requiredMemorySize(MEMORY_MINBLOCKSIZE, MEMORY_MINBLOCKSIZE);

			break;
		}
		default:
			break;
	}

	while (bytes > minAllocatableBytes)
	{
	
		unsigned int bytesUsed = mem_createMemoryPool(&gMemoryPools[gNumMemoryPools], (void *)freeAddress, bytes, MEMORY_POOLTYPE);

		if (bytesUsed == 0)
		{
			kprintf("Could not create a memory pool...\n");
			return false;
		}

		freeAddress += bytesUsed;

		bytes -= bytesUsed;

		gNumMemoryPools ++;				

		//if (gNumMemoryPools > 3)
		//	break;
	
	}	

	// Now add all other memory blocks that are available.


	for (int j = 0; j < memMapSize; j ++)
	{
	
		if (j == i)	// ignore the block we have already partitioned
			continue;

		if ( ! (memMap[j].baseAddress >= KERNEL_LOAD_ADDRESS) ||
		     ! (memMap[j].baseAddress <  0xffffffffU) ||
  		     ! (memMap[j].type == MEMORY_TYPE_FREE) )
			continue;	// unsuitable memory			
	
		freeAddress = memMap[i].baseAddress & 0xffffffffU;
		
		bytes = (unsigned int)(MIN(0xffffffffU - freeAddress, memMap[j].size));

		while (bytes > minAllocatableBytes)
		{
		
			unsigned int bytesUsed = mem_createMemoryPool(&gMemoryPools[gNumMemoryPools], (void *)freeAddress, bytes, MEMORY_POOLTYPE);

			if (bytesUsed == 0)
			{
				kprintf("Could not create a memory pool...\n");
				return false;
			}

			freeAddress += bytesUsed;

			bytes -= bytesUsed;

			gNumMemoryPools ++;				
		
		}	
	
	}	


	return true;

}

unsigned int mem_estimateNumberOfMemoryPools(enum MemoryPoolType type, unsigned int size)
{

	switch (type)
	{
		case MEMORYPOOLTYPE_BUDDY:
			return mem_buddy_estimateNumberOfBuddyAllocatorsForRegion(size, MEMORY_MINBLOCKSIZE);
		default:
			break;
	}

	return 0;

}

unsigned int mem_createMemoryPool(struct MemoryPool* pool, void* baseAddress, unsigned int size, enum MemoryPoolType type)
{
	switch (type)
	{
		case MEMORYPOOLTYPE_BUDDY:
			return mem_buddy_createBuddyMemoryPool(pool, (uintptr_t)baseAddress, size, MEMORY_MINBLOCKSIZE);
		default:
			return false;
	}
}

void*	kmalloc(unsigned int bytes, enum MemoryType type)
{

	unsigned int bytesAllocated = bytes;

	//
	//	Find a free block that's big enough
	//

	void* block = NULL;


	struct MemoryPool* pools = (type == MEMORYTYPE_HIMEM) ? gMemoryPools : gLowMemoryPools;
	int count = (type == MEMORYTYPE_HIMEM) ? gNumMemoryPools : gNumLowMemoryPools;

	for (int i = 0; i < count; i ++)
	{
		struct MemoryPool* pool = &pools[i];
	
		kprintf("Trying pool %08x %08x\n", pool, pool->allocMemory);

		if (pool->allocMemory != NULL)
			block = pool->allocMemory(pool, &bytesAllocated);

		if (block != NULL)
			break;
	}

	kprintf("Allocated block %08x %d\n", block, bytesAllocated);

	if (block == NULL)
	{
		return NULL;
	}

	//
	//	temporarily page in the memory so we can access it
	//


	// Memory allocated by the kernel could end up in user space,
	// and it would be a security nightmare if we didn't zero all
	// the memory.

	void* blockVirtual = block; // map function here.

	lib_bzero(blockVirtual, bytesAllocated);

	//
	//	remove memory from page tables (if it is needed
	//	in the kernel, it will be mapped appropriately,
	//	otherwise if it is to be used in a user process,
	//	it will be added to that process' page tables.
	//

	return block;
}

void	kfree(void* memory)
{

	// TODO

}

bool	mem_initLowmem(void* lowMemStart, unsigned int lowMemBytes)
{

	// Paging: we assume the low memory has already been identity mapped in the page tables.

	struct MemoryPool* pool = (struct MemoryPool*) lowMemStart;

	unsigned int reservedBytes = sizeof(struct MemoryPool); 
 
	uintptr_t dataStart = (uintptr_t)pool + reservedBytes;

	if (mem_kr_createMemoryPool(pool, (void*)dataStart, lowMemBytes - reservedBytes) == 0)
	{
		kprintf("could not create KR memory pool\n");
		return false;
	}

	gLowMemoryPools     = pool;
	gNumLowMemoryPools  = 1;

	return true;
}

