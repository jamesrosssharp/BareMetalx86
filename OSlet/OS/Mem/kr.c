
#include "mem.h"
#include "kr.h"

bool	mem_kr_init(void* mem, unsigned int memSize)
{
	
	if (memSize < (sizeof(struct KRMemoryAllocator) + sizeof(struct KRBlockHeader)))
		return false;


	struct KRMemoryAllocator* krAlloc = (struct KRMemoryAllocator*)mem;

	krAlloc->size = memSize;
	krAlloc->units = memSize / sizeof(struct KRBlockHeader);

	struct KRBlockHeader* head = (struct KRBlockHeader*)krAlloc->data;

	krAlloc->head = head;

	head->size = (memSize - sizeof(struct KRMemoryAllocator) - sizeof(struct KRBlockHeader)) / sizeof(struct KRBlockHeader);

	head->next = head;

	return true;

}

void*	mem_kr_allocate(struct KRMemoryAllocator* kr, unsigned int bytes)
{

	struct KRBlockHeader* node = kr->head;
	struct KRBlockHeader* prevNode = NULL;

	unsigned int units = (bytes + sizeof(struct KRBlockHeader) - 1) / sizeof(struct KRBlockHeader) + 1; // includes header

	do
	{
	
		if (node == NULL)
			break;
	
		if (node->size == units)
		{
			
			if (prevNode == NULL)
			{
				kr->head = NULL; // all memory used
			}
			else
			{
				prevNode->next = node->next;
			}

			return (void*) (node + 1);

		}
		else if (node->size > units)
		{

			// add the block to the tail of the current node

			struct KRBlockHeader* p = node + node->size - units;

			p->next = node->next;
			p->size = units;

			node->size -= units;
			
			return (void*) (p + 1);

		}

		prevNode = node;
		node = node->next;
	} while (node != kr->head);

	return NULL;

}

void 	mem_kr_debug(struct KRMemoryAllocator* kr)
{

	struct KRBlockHeader* node = (struct KRBlockHeader*)kr->data;

	int n = 0;
	
	for (; (uintptr_t) node < ((uintptr_t)kr->data + kr->units * sizeof(struct KRBlockHeader)); node += node->size)
	{

		if (node->size == 0)
			break;

		#define MAKE_ADDR(x) (((uintptr_t)x - (uintptr_t)kr->data) / sizeof(struct KRBlockHeader))

		DEBUG("\tNode: %lx -> %lx (%d units of size %lu)\n", MAKE_ADDR(node), 
				MAKE_ADDR(node->next), node->size, sizeof(struct KRBlockHeader));

		n ++;
	} 

	DEBUG("%d nodes\n", n);

}

void	mem_kr_free(struct KRMemoryAllocator* kr, void* address)
{

	DEBUG("free: %p %p\n", kr, address);

	struct KRBlockHeader* node = (struct KRBlockHeader*)address - 1;

	// search freelist for block

	struct KRBlockHeader* p = kr->head;

	for (; ! (node > p && node < p->next ); p = p->next) // find a p which spans node
	{
		if (p >= p->next && (node > p || node < p->next)) // if p->next wraps around the list, and node is either at the end of the
								  // list or before the start of the list, break (we have found p, the previous
								  // link in the chain)
			break;
	}


	if (node + node->size == p->next) // if the next block is the one pointed to by p, it is free, so coalesce it with this one
	{
		node->size += p->next->size;
		node->next = p->next->next;
	} 
	else
		node->next = p->next; // else simply point to next block

	if (p + p->size == node) // if the current block is equal to p plus its size (the two blocks are adjacent), coalesce them
	{
		p->size += node->size;
		p->next = node->next;
	} 
	else
		p->next = node;  // else simply point to this block in the free list
	  
	kr->head = p;	// set head of the list to the previous node
		
}

void* mem_kr_allocMemoryFromMemoryPool(struct MemoryPool* memPool, unsigned int* size)
{
	
	struct KRMemoryAllocator* kr = (struct KRMemoryAllocator*) memPool->allocatorVirtual;

	return mem_kr_allocate(kr, *size);

}

void  mem_kr_freeMemoryFromMemoryPool(struct MemoryPool* memPool, void* memory)
{
	
	struct KRMemoryAllocator* kr = (struct KRMemoryAllocator*) memPool->allocatorVirtual;

	return mem_kr_free(kr, memory);

}

bool mem_kr_belongsToMemoryPool(struct MemoryPool* memPool, void* memory)
{

	return ((memory >= memPool->baseAddress) && (memory < memPool->baseAddress + memPool->size));		

}

unsigned int mem_kr_createMemoryPool(struct MemoryPool* pool, void* dataStart, unsigned int size)
{

	if (! mem_kr_init(dataStart, size))
		return 0;

	pool->type = MEMORYPOOLTYPE_KR;
	pool->baseAddress = (void*) dataStart;
	pool->size = size;	

	pool->allocatorPhys = (void*) dataStart;
	pool->allocatorVirtual = (void*) dataStart; // this should be the virtual address of the KR structures.
						    // the whole of the KR allocator must be paged into kernel space,
						    // as each block will be accessed each time an allocation / free
						    // is performed.

		// fill in function pointers

	pool->allocMemory = mem_kr_allocMemoryFromMemoryPool;
	pool->freeMemory =  mem_kr_freeMemoryFromMemoryPool;
	pool->belongsTo  =  mem_kr_belongsToMemoryPool;

	return size;

}
