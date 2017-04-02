/*

	Free list memory allocator


					*/

#include "freelist.h"

#define FREE_LIST_ALIGN 4

int	mem_freeList_requiredMemorySize(int blockSize, int numberOfBlocks)
{
	return sizeof(struct FreeList) + FREE_LIST_ALIGN + numberOfBlocks * blockSize;
}

//
//	Must pass in block of memory of size reported
//	by mem_freeList_requiredMemorySize();
//
bool	mem_freeList_init(int blockSize, int numberOfBlocks, void* memory)
{

	DEBUG("Initing free list %p %d %d\n", memory, blockSize, numberOfBlocks);

	if ((blockSize < sizeof(uintptr_t)) || (numberOfBlocks < 1))
		return false;

	struct FreeList* list = (struct FreeList*)memory;
	
	list->blockSize = blockSize;

	// Align pointer to free list to	

	uintptr_t* head = (uintptr_t*) (((uintptr_t)list + sizeof(struct FreeList) + FREE_LIST_ALIGN - 1) & ~(FREE_LIST_ALIGN - 1));

	list->head = head;

	DEBUG(" head: %p \n", list->head);

	uintptr_t* next;

	for (int i = 0; i < numberOfBlocks - 1; i++)
	{
		next = (uintptr_t*)((uintptr_t)head + blockSize); 
		*head = (uintptr_t)next; 
		head = next;
	}

	*head = (uintptr_t)NULL;

	return true;	
}

void*	mem_freeList_allocateBlock(void* memory)
{

	struct FreeList* list = (struct FreeList*)memory;

	DEBUG("Allocating: free list: %p\n", memory);

	if (list->head == NULL)
		return NULL;
	
	uintptr_t* head = (uintptr_t*)list->head;
	list->head = (void*)*head;

	DEBUG("  block: %p head: %p\n", head, list->head);

	return head;
	
}

void	mem_freeList_freeBlock(void* memory, void* block)
{
	struct FreeList* list = (struct FreeList*)memory;

	DEBUG("Freeing block: mem: %p block: %p\n", memory, block);

	uintptr_t* head = (uintptr_t*)block;

	*head = (uintptr_t)list->head;
	
	list->head = block;
}

