
#include "biosmem.h"
#include "../IO/io.h"
#include "../Console/console.h"
#include "../Lib/string.h"
#include "../Mem/mem.h"

#define MAX_MEMORY_ENTRIES 32

struct MemoryEntry gMemoryEntries[MAX_MEMORY_ENTRIES] = {0};
int	gNumMemoryEntries = 0;

bool	bios_detectMemory(struct MemoryEntry** entries, int* numEntries)
{

	//kprintf("Detecting memory...\n");


	struct MemoryEntry* lowMemEntry = kmalloc(sizeof(struct MemoryEntry), MEMORYTYPE_LOMEM);

	if (lowMemEntry == NULL)
		return false;

	struct MemoryEntry* curEntry = gMemoryEntries;
	gNumMemoryEntries = 0;

	struct RegisterDescription in = {0};
	struct RegisterDescription out = {0};

	out.EBX = 0;

	unsigned long long int totalMemory = 0;

	int i;
	for (i = 0; i < MAX_MEMORY_ENTRIES; i++)
	{
	
		in.EAX = 0xE820;
		in.EDX = 0x534D4150;
		in.EBX = out.EBX;
		in.ECX = 24;

		in.ES = (((uintptr_t)lowMemEntry) & 0xffff0000) >> 4;
		in.EDI = (((uintptr_t)lowMemEntry) & 0x0000ffff); 

		bool success = io_realModeInt(0x15, &in, &out);	

		if (! success)
		{
			kprintf("Failed to detect memory.\n");
			kfree(lowMemEntry);
			return false;
		}
		else
		{

			lib_memcpy(curEntry, lowMemEntry, sizeof(struct MemoryEntry));

			//kprintf("Found memory: base 0x%08x%08x size 0x%08x%08x flags: 0x%x\n", 
			//	(unsigned int)(curEntry->baseAddress >> 32), (unsigned int)(curEntry->baseAddress & 0xffffffff),
			//	(unsigned int)(curEntry->size >> 32), (unsigned int)(curEntry->size & 0xffffffff),
			//	curEntry->type	
			//);

		}

		if (curEntry->type == 1)
		{
			totalMemory += curEntry->size;
		}

		if (out.EBX == 0)
			break;

		curEntry ++;

	}

	//kprintf("Detected memory (%d MiB)\n", (int)(totalMemory / 1024 / 1024));

	*entries = gMemoryEntries;
	*numEntries = i;

	kfree(lowMemEntry);

	return true;
}
