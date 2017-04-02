
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

typedef void (*BTreeTraverseFunction) (void* data, void* nodeData, int depth);

#ifdef __cplusplus
extern "C" {
#endif

int lib_btree_maxElementsRequired(int depth);
int lib_btree_requiredMemorySize(int maxDepth);
bool lib_btree_init(void* memory, int maxDepth);
struct BTreeNode* lib_btree_findLeaf(struct BTreeNode* node, int bisector, int *depth);
bool lib_btree_addElement(struct BTree* tree, int bisector, void* data);

void	lib_btree_debugNode(struct BTreeNode* node, int depth);
void	lib_btree_debugTree(struct BTree* tree);

void 	lib_btree_traverseTreeWithCallback(struct BTree* tree, bool leavesOnly, BTreeTraverseFunction func, void* data);

#ifdef __cplusplus
}
#endif
