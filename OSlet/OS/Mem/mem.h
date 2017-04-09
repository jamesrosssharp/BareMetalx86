#pragma once

#include "../kerndefs.h"

#include "freelist.h"
#include "buddy.h"

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

	// Pointer to an allocator structure, e.g. BuddyMemoryAllocator, in physical memory
	void* allocatorPhys; 

	// virtual address of allocator in Kernel virtual memory space. (The actual allocator structures
	// must be paged into kernel space so we can access them)
	void* allocatorVirtual;	

	// Function pointer to allocate from memory pool
	void* (* allocMemory) (struct MemoryPool* memPool, unsigned int size);		
	void  (* freeMemory) (struct MemoryPool* memPool, void* memory);	

};

bool	mem_init();
