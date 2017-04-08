/*

	A binary tree implementation. The entire tree
	is preallocated and memory is used from a 
	preallocated memory pool. Tree cannot grow beyond a 
	maximum depth.

	For a dynamic BTree, use DynamicBTree, which
	uses a dynamic memory pool which can increase 
	in size.


								*/

#include "btree.h"
#include "../Mem/mem.h"

#define BTREE_ALIGN 4

#define IS_LEAF(node) ((node->left == NULL) && (node->right == NULL))

//	return max elements required for a given tree depth
int lib_btree_maxElementsRequired(int depth)
{


	// Compute 2^N - 1

	return (1 << depth) - 1;
}

// return maximum memory required for a given max depth
int lib_btree_requiredMemorySize(int maxDepth)
{
	int elem = lib_btree_maxElementsRequired(maxDepth);

	return sizeof(struct BTree) + mem_freeList_requiredMemorySize(sizeof(struct BTreeNode), elem) + BTREE_ALIGN;

}

// initialise a BTree structure. Enough memory must be allocated to hold the entire structure -
// use lib_btree_requiredMemorySize to compute the space requirements

bool lib_btree_init(void* memory, int maxDepth)
{

	struct BTree* tree = (struct BTree*)memory;

	tree->maxDepth = maxDepth;	

	int elem = lib_btree_maxElementsRequired(maxDepth);

	void* freeList = (void*)(((uintptr_t)memory + sizeof(struct BTree) + BTREE_ALIGN - 1) & ~(BTREE_ALIGN - 1));

	tree->elementPool = freeList;

	if (! mem_freeList_init(sizeof(struct BTreeNode), elem, freeList))
		return false;

	tree->root = NULL;	// set root element to null, as we haven't added a root yet.

	return true;

}

// Find a leaf for a given bisector

struct BTreeNode* lib_btree_findLeaf(struct BTreeNode* node, int bisector, int *depth)
{

	*depth++;

	if (bisector < node->bisector)
	{

		if (node->left == NULL)
			return node;
		else
			return lib_btree_findLeaf(node->left, bisector, depth);
	}
	else if (bisector > node->bisector)
	{
		if (node->right == NULL)
			return node;
		else
			return lib_btree_findLeaf(node->right, bisector, depth);
	}
	else
		return node;

}

// Add an element to a BTree

bool lib_btree_addElement(struct BTree* tree, int bisector, void* data)
{

	DEBUG("Adding element %p %d %p\n", tree, bisector, data);

	// allocate a node
	struct BTreeNode* newNode = (struct BTreeNode*) mem_freeList_allocateBlock((void*)tree->elementPool);

	if (newNode == NULL)
		return false;

	newNode->left = newNode->right = NULL;
	newNode->bisector = bisector;
	newNode->data = data;

	if (tree->root == NULL)
	{
	   // we have no tree yet. Add this as the first node.

	   tree->root = newNode;

	}
	else
	{
		int depth = 0;
		struct BTreeNode* node = lib_btree_findLeaf(tree->root, bisector, &depth);

		newNode->parent = node;
		
		if (depth > tree->maxDepth)
		{
			DEBUG("Tree depth exceeded\n");
			mem_freeList_freeBlock((void*)tree->elementPool, newNode);
			return false;
		}

		if (node->bisector == bisector)
		{
			DEBUG("Node %d already present\n", bisector);
			mem_freeList_freeBlock((void*)tree->elementPool, newNode);
			return false;
		}

		if (IS_LEAF(node))
		{
			if (bisector < node->bisector)
				node->left = newNode;
			else
				node->right = newNode;
		
		}
		else
		{
			// node is not a leaf. Work out which side of the node's
			// bisector we are on. Then add our new node.

			if (bisector <= node->bisector)
				node->left = newNode;
			else
				node->right = newNode;
		
		}	
	}

	return true;
}

void	lib_btree_debugNode(struct BTreeNode* node, int depth)
{
	if (node == NULL)
		return;

	#define DEBUG_INDENT	for (int i = 0; i < depth; i ++) DEBUG(" "); 


	DEBUG_INDENT
	DEBUG("Node: %d %p l: %p r: %p\n", node->bisector, node->data, node->left, node->right);

	DEBUG_INDENT
	DEBUG("L:\n");
	lib_btree_debugNode(node->left, depth + 1);
	
	DEBUG_INDENT
	DEBUG("R:\n");	
	lib_btree_debugNode(node->right, depth + 1);

}

void	lib_btree_debugTree(struct BTree* tree)
{
	lib_btree_debugNode(tree->root, 0);
}

void	lib_btree_traverseNodeWithCallback(struct BTreeNode* node, int depth, bool leavesOnly, BTreeTraverseFunction func, void* data)
{
	if (node == NULL)
		return;

	if (! leavesOnly || IS_LEAF(node))
		func(node, data, depth);
	
	lib_btree_traverseNodeWithCallback(node->left, depth + 1, leavesOnly, func, data);
	lib_btree_traverseNodeWithCallback(node->right, depth + 1, leavesOnly, func, data);

}

void 	lib_btree_traverseTreeWithCallback(struct BTree* tree, bool leavesOnly, BTreeTraverseFunction func, void* data)
{
	lib_btree_traverseNodeWithCallback(tree->root, 0, leavesOnly, func, data); 
}

void*	lib_btree_getLeftNodeData(struct BTreeNode* node)
{

	struct BTreeNode* left = node->left;

	return left->data;

}

void*	lib_btree_getRightNodeData(struct BTreeNode* node)
{
	struct BTreeNode* right = node->right;

	return right->data;
}

void*	lib_btree_getParent(struct BTreeNode* node)
{
	return node->parent;
}

void	lib_btree_makeNodeALeaf(struct BTree* tree, struct BTreeNode* node)
{
	if (node == NULL)
		return;

	lib_btree_makeNodeALeaf(tree, node->left);
	lib_btree_makeNodeALeaf(tree, node->right);

	if (node->left != NULL)
	{
		mem_freeList_freeBlock(tree->elementPool, node->left);
		node->left = NULL;
	}
	
	if (node->right != NULL)
	{
		mem_freeList_freeBlock(tree->elementPool, node->right);
		node->right = NULL;
	}

}

void*	lib_btree_getNodeData(struct BTreeNode* node)
{
	return node->data;
}		
