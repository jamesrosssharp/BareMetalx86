//
//
//	buddytest: test the buddy memory allocator
//
//
//

#include "../../../OS/Mem/kr.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char** argv)
{
	const int memSize = 4*1024*1024;
	const int minBlockSize = 4096;

	unsigned char* mem = new unsigned char [memSize];

	const int blocks = 100;

	void* blockPtr[blocks];

	mem_kr_init(mem, memSize);

	for (int i = 0; i < blocks; i++)
	{
		int bytes = rand() % (memSize / 100);

		void* addr = mem_kr_allocate((struct KRMemoryAllocator*)mem, bytes);

		if (! addr)
			cout << "Failed to allocate memory!" << endl;

		blockPtr[i] = addr;
	}

	cout << "!!!!!!!!!!!!!!!!!!! After alloc !!!!!!!!!!!!!!!!!" << endl;

	mem_kr_debug((struct KRMemoryAllocator*)mem);	

	int blocksFreed = 0;

	while (blocksFreed < blocks / 2)
	{

		int block = rand() % blocks;

		void* address = blockPtr[block];

		if (address)
		{
			mem_kr_free((struct KRMemoryAllocator*)mem, address);

			blockPtr[block] = NULL;
			blocksFreed ++;
		}
	}

	cout << "!!!!!!!!!!!!!!!!!!! After free !!!!!!!!!!!!!!!!!" << endl;

	mem_kr_debug((struct KRMemoryAllocator*)mem);

}
