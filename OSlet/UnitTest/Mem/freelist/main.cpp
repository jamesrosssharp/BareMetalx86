//
//
//	freelisttest: test the free list in the kernel memory library
//
//
//

#include <iostream>

#include "../../../OS/Mem/freelist.h"

using namespace std;

extern "C"
{

	int	mem_freeList_requiredMemorySize(int blockSize, int numberOfBlocks);
	bool	mem_freeList_init(int blockSize, int numberOfBlocks, void* memory);
	void*	mem_freeList_allocateBlock(void* memory);
	void	mem_freeList_freeBlock(void* freeList, void* block);
}


int main(int argc, char** argv)
{

	const int blockSize = 32;
	const int numBlocks = 1024; 

	void* blocks[numBlocks] = {0};	

	int memSize = mem_freeList_requiredMemorySize(blockSize, numBlocks);

	cout << "Allocating " << memSize << " bytes for free list" << endl;

	unsigned char* mem = new unsigned char [memSize];

	// Allocate until we get a null value back for the allocation.	

	mem_freeList_init(blockSize, numBlocks, mem);

	for (int i = 0; i < 10; i ++)
	{

		cout << "============= Trial " << i << " ===========" << endl;

		int j = 0;
		while(blocks[j++] = mem_freeList_allocateBlock(mem));	

		// free blocks in random order.
		
		while (j > 1)
		{

			int block = rand() % numBlocks;

			if (blocks[block] != nullptr)
			{
				mem_freeList_freeBlock(mem, blocks[block]);	
				blocks[block] = nullptr;
				j --;
			}
		}

		// traverse linked list and print

		for (void* ptr = ((struct FreeList*)mem) -> head; ptr; ptr = *(void**)ptr)
		{
			cout << "Node: " << ptr << endl;
		}
	}

	delete [] mem;

}

