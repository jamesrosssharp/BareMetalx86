
#pragma once

#include "klib.h"
#include "../Mem/mem.h"

struct BTreeNode
{
	int bisector;

	void*	data;

	struct BTreeNode* left;
	struct BTreeNode* right;

};

struct BTree
{

	struct FreeList* elementPool;
	int	maxDepth;
	struct BTreeNode* root;

};
