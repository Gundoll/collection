#ifndef __GD_B_PLUS_TREE_H__
#define __GD_B_PLUS_TREE_H__

#include "collection.h"

struct BpIndexNode {
	bool				isLeaf;
	void**				indices;
	struct BpTreeNode** children;
};

struct BpLeafNode {
	bool				isLeaf;
	void**				values;
	struct BpLeafNode*	next;
};

struct GDBpTree {
	GD_COLLECTION_PROPS
	size_t				degree;
	struct BpTreeNode*	headNode;
	struct BpLeafNode*	leafNode;
};

struct GDBpTree* GDBpTreeCreate(size_t degree, size_t limitSize, struct Context context);
void GDBpTreeDestroy(struct GDBpTree* self);

#endif
