#pragma once

#include "../kerndefs.h"

#include "freelist.h"

enum MemoryPoolType
{
	MEMORYPOOLTYPE_BUDDY,
};

struct MemoryPool
{

	enum MemoryPoolType type;

	// The parameters of the memory block, detected by BIOS, and truncated to 32 bits.
	void* baseAddress;
	unsigned int size;

	// Some of the pool will be reserved. This is the address of the allocatable memory.
	void* baseAddressAllocatable;
	unsigned int sizeAllocatable;

	// Pointer to reserved memory start
	void* baseAddressReserved;
	unsigned int sizeReserved;

	// Function pointer to allocate from memory pool
	void* (* allocMemory) (struct MemoryPool* memPool, unsigned int size);		
};


bool mem_createMemoryPool(struct MemoryPool* pool, void* baseAddress, unsigned int size, enum MemoryPoolType type);
bool	mem_init();
