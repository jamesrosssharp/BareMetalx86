
#include "mem.h"
#include "../Console/console.h"
#include "../BIOS/bios.h"

bool mem_createMemoryPool(struct MemoryPool* pool, void* baseAddress, unsigned int size, enum MemoryPoolType type);

struct MemoryPool *gMemoryPools;
int		   gNumMemoryPools = 0;

bool	mem_init()
{

	kprintf("Initing memory subsystem...");	

	// Query BIOS memory map

	struct MemoryEntry* memoryMap;

	int memMapSize = 0;
	struct MemoryEntry* memMap = 0; 

	if (! bios_detectMemory(&memMap, &memMapSize) || memMapSize == 0)
	{
		kprintf("Could not detect memory map...");
		return false;
	}

	kprintf("memMap: %08x\n", memMap);

	int i;

	for (i = 0; i < memMapSize; i ++)
	{
		// We want memory above 1 Meg.
		if ((memMap[i].baseAddress >= 0x100000) &&
		    (memMap[i].baseAddress <  0xffffffff) &&
  		    (memMap[i].type == MEMORY_TYPE_FREE) )
		{
			kprintf("Found usable memory at %08x%08x %d\n", 
				(unsigned int)(memMap[i].baseAddress >> 32), 
				(unsigned int)(memMap[i].baseAddress & 0xffffffff), i);
			break;
		}		
	}

	if (i < memMapSize)
	{
		// The first chunk we use to hold an array of memory pools

		// First we need to determine how many memory pools to create

		gNumMemoryPools = 1;

		for (int j = i + 1; j < memMapSize; j ++)
		{
			if ((memMap[j].baseAddress >= 0x00100000) &&
			    (memMap[j].baseAddress <  0xffffffff) &&
			    (memMap[j].type == MEMORY_TYPE_FREE))
			{

				// we have a useable memory block. Need to determine how many Buddy allocators will fit in
				// the memory block.

				//	gNumMemoryPools ++;
			}
		}

		int reserveBytes = sizeof(struct MemoryPool) * gNumMemoryPools;

		kprintf("Reserving %d bytes for %d memory pools.\n", reserveBytes, gNumMemoryPools);

		gMemoryPools = (struct MemoryPool*)(unsigned int)(memMap[i].baseAddress & 0xffffffff);

		// Recursively divide the memory block up into memory pools.




		void* firstMemPoolBaseAddress = (void*)(((unsigned int)gMemoryPools + reserveBytes + 3) & ~3); // align memory pool base address 

		// get max size of first memory pool

		unsigned int firstMemBlockSize = MIN((unsigned int)memMap[i].baseAddress + memMap[i].size - (unsigned int)firstMemPoolBaseAddress, 0xffffffff - (unsigned int)memMap[i].baseAddress); 
	
		

		// Create a memory pool 

		bool ret = mem_createMemoryPool(gMemoryPools, firstMemPoolBaseAddress, firstMemPoolSize, MEMORYPOOLTYPE_BUDDY);

		if (! ret)
		{
			kprintf("Could not init memory pool.\n");
			return false;
		}


	}
	else
	{
		kprintf("System does not have any memory above 1MiB free for use.\n");
		return false;
	}

	// Now for all remaining regions, recursively divide up into Memory pools.

	
		
}


bool mem_createMemoryPool(struct MemoryPool* pool, void* baseAddress, unsigned int size, enum MemoryPoolType type)
{
	kprintf("Creating memory pool: %x, %x, %d, %d\n", pool, baseAddress, size, type);

	return false;
}
