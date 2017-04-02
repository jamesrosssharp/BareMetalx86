//
//
//	btreetest: test the binary tree implementation in the kernel lib
//
//
//

#include "../../../OS/Lib/btree.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

extern "C"
{

	int 	lib_btree_maxElementsRequired(int depth);
	int 	lib_btree_requiredMemorySize(int maxDepth);
	bool 	lib_btree_init(void* memory, int maxDepth);
	struct BTreeNode* lib_btree_findLeaf(struct BTreeNode* node, int bisector, int *depth);
	bool lib_btree_addElement(struct BTree* tree, int bisector, void* data);
	void 	lib_btree_debugTree(struct BTree* tree);
}


int main(int argc, char** argv)
{

	const int depth = 10;

	int elem = lib_btree_maxElementsRequired(depth);

	int memSize = lib_btree_requiredMemorySize(depth);

	cout << "BTree with depth = " << depth << " will consume " << memSize << " bytes (" << elem << " elements)" << endl;	 

	unsigned char* mem = new unsigned char[memSize];

	if (! lib_btree_init(mem, depth))
	{
		delete [] mem;
		exit(EXIT_FAILURE);
	}

	int elemAdded = 0;

	while (elemAdded < elem)
	{
		if (lib_btree_addElement((struct BTree*)mem, rand() % (1<<depth), (void*)(uintptr_t)rand()))
			elemAdded++;
	}

	lib_btree_debugTree((struct BTree*)mem);

	delete[] mem;

	exit(EXIT_SUCCESS);
}

