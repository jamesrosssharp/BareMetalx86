
#pragma once

#include "mem.h"

struct FreeList
{
	int	blockSize;
	void	*head;
};

int	mem_freeList_requiredMemorySize(int blockSize, int numberOfBlocks);
bool	mem_freeList_init(int blockSize, int numberOfBlocks, void* memory);
void*	mem_freeList_allocateBlock(void* memory);
void	mem_freeList_freeBlock(void* memory, void* block);

