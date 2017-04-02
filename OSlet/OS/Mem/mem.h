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

	// Base address and size of the memory pool
	void* baseAddress;
	unsigned int size;

	// Pointer to an allocator structure, e.g. BuddyMemoryAllocator
	void* allocator; 

	// Function pointer to allocate from memory pool
	void* (* allocMemory) (struct MemoryPool* memPool, unsigned int size);		

	void  (* freeMemory) (struct MemoryPool* memPool, void* memory);

};



bool mem_createMemoryPool(struct MemoryPool* pool, void* baseAddress, unsigned int size, enum MemoryPoolType type);
bool	mem_init();
