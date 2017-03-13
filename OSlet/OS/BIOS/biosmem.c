
#include "biosmem.h"
#include "../IO/io.h"
#include "../Console/console.h"

#define MAX_MEMORY_ENTRIES 32

struct MemoryEntry gMemoryEntries[MAX_MEMORY_ENTRIES] = {0};
int	gNumMemoryEntries = 0;

bool	bios_detectMemory(struct MemoryEntry** entries, int* numEntries)
{

	kprintf("Detecting memory...\n");

	struct MemoryEntry* curEntry = gMemoryEntries;
	gNumMemoryEntries = 0;

	struct RegisterDescription in = {0};
	struct RegisterDescription out = {0};

	for (int i = 0; i < MAX_MEMORY_ENTRIES; i++)
	{
	
		in.EAX = 0xE820;
		in.EDX = 0x534D4150;
		in.EBX = 0;
		in.ECX = 24;

		in.ES = (((int)curEntry) & 0xffff0000) >> 4;
		in.EDI = (((int)curEntry) & 0x0000ffff); 

		bool success = io_realModeInt(0x15, &in, &out);	

		if (! success)
		{
			kprintf("Failed to detect memory.\n");
			return false;
		}

		break;

	}


	return true;
}
