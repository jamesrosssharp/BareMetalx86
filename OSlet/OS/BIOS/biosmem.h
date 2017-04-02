
#pragma once

#include "../kerndefs.h"

#define MEMORY_TYPE_FREE 0x01

struct MemoryEntry
{
	unsigned long long int	baseAddress;
	unsigned long long int 	size;
	unsigned int type;
	unsigned int acpi3ExtendedAttributes;
};

bool	bios_detectMemory(struct MemoryEntry** entries, int* numEntries);

