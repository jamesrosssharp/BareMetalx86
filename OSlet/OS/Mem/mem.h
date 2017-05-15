#pragma once

#include "../kerndefs.h"


enum MemoryPoolType
{
	MEMORYPOOLTYPE_BUDDY,
	MEMORYPOOLTYPE_KR,
};

enum MemoryType
{
	MEMORYTYPE_LOMEM,
	MEMORYTYPE_HIMEM
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
	void* (* allocMemory) (struct MemoryPool* memPool, unsigned int* size);		
	void  (* freeMemory) (struct MemoryPool* memPool, void* memory);	

};


bool	mem_initHimem();
bool	mem_initLowmem(void* lowMemStart, unsigned int lowMemBytes);

void*	kmalloc(unsigned int bytes, enum MemoryType type);
void	kfree(void* memory);


#include "buddy.h"
#include "freelist.h"
#include "kr.h"
