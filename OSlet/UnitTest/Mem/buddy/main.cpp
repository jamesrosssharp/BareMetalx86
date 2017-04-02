//
//
//	buddytest: test the buddy memory allocator
//
//
//

#include "../../../OS/Mem/buddy.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char** argv)
{

	const int memSize = 4*1024*1024;
	const int minBlockSize = 4096;


	int maxBlockSize = mem_buddy_maxBuddyBlockSizeForMemoryRegion(memSize, minBlockSize);

	cout << "Buddy allocator for " << memSize / 1024 / 1024 << " MiB region has max block size " << maxBlockSize << endl;	

	unsigned char* mem = new unsigned char [memSize];

	mem_buddy_init(maxBlockSize, minBlockSize, (void*) mem);

	mem_buddy_debug((struct BuddyMemoryAllocator*)mem);	

}
