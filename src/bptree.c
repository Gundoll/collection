#include "bptree.h"

static RetCode_t BpIndexNodeCreate(struct BpIndexNode** node, size_t degree) {
	if(node == NULL)
		return RETCODE_INVALID_ARGS;

	struct BpIndexNode* self = (struct BpIndexNode*)malloc(sizeof(struct BpIndexNode));
	if(self == NULL)
		goto error;

	self->isLeaf = false;
	self->indices = malloc((degree - 1) * sizeof(void*));
	self->children = malloc((degree) * sizeof(void*));
	if(self->indices == NULL || self->children == NULL)
		goto error;

	*node = self;

	return RETCODE_OK;

error:
	if(self) {
		if(self->indices)
			free(self->indices);
		if(self->children)
			free(self->children);
		free(self);
	}
	return RETCODE_NOT_ENOUGH_RESOURCES;
}

static RetCode_t BpLeafNodeCreate(struct BpLeafNode** node, size_t degree, size_t elementSize) {
	if(node == NULL)
		return RETCODE_INVALID_ARGS;

	struct BpLeafNode* self = (struct BpLeafNode*)malloc(sizeof(struct BpLeafNode));
	if(self == NULL)
		goto error;

	self->isLeaf = true;
	self->next = NULL;
	self->values = calloc((degree - 1), elementSize);
	if(self->values == NULL)
		goto error;

	*node = self;

	return RETCODE_OK;

error:
	if(self) {
		if(self->values)
			free(self->values);
		free(self);
	}
	return RETCODE_NOT_ENOUGH_RESOURCES;
}

RetCode_t GDBpTreeIsEmpty(void* self) {
	struct GDBpTree* tree = (struct GDBpTree*)self;
	if(self == NULL)
		return RETCODE_INVALID_ARGS;
	if(tree->size == 0)
		return RETCODE_OK;
	return RETCODE_ERROR;
}

RetCode_t GDBpTreeContains(void* self, uintptr_t element) {
	struct GDBpTree* tree = (struct GDBpTree*)self;
	if(self == NULL)
		return RETCODE_INVALID_ARGS;
	if(tree->size == 0)
		return RETCODE_EMPTY;

	return RETCODE_OK;
}

RetCode_t GDBpTreeAdd(void* self, uintptr_t element) {
	struct GDBpTree* tree = (struct GDBpTree*)self;
	if(self == NULL)
		return RETCODE_INVALID_ARGS;

	return RETCODE_OK;
}

RetCode_t GDBpTreeGet(void* self, uint32_t index, void* value) {
	struct GDBpTree* tree = (struct GDBpTree*)self;
	if(self == NULL)
		return RETCODE_INVALID_ARGS;
	if(tree->size == 0)
		return RETCODE_EMPTY;

	return RETCODE_OK;
}

RetCode_t GDBpTreeRemove(void* self, uintptr_t element) {
	struct GDBpTree* tree = (struct GDBpTree*)self;
	if(self == NULL)
		return RETCODE_INVALID_ARGS;

	return RETCODE_OK;
}

RetCode_t GDBpTreeRemoveAll(void* self) {
	struct GDBpTree* tree = (struct GDBpTree*)self;
	if(self == NULL)
		return RETCODE_INVALID_ARGS;

	return RETCODE_OK;
}

RetCode_t GDBpTreePurge(void* self, uintptr_t element) {
	struct GDBpTree* tree = (struct GDBpTree*)self;
	if(self == NULL)
		return RETCODE_INVALID_ARGS;

	return RETCODE_OK;
}

RetCode_t GDBpTreePurgeAll(void* self) {
	struct GDBpTree* tree = (struct GDBpTree*)self;
	if(self == NULL)
		return RETCODE_INVALID_ARGS;

	return RETCODE_OK;
}

struct GDBpTree* GDBpTreeCreate(size_t degree, size_t limitSize, struct Context context) {
	// Step 01: memory allocation
	struct GDBpTree* self = (struct GDBpTree*)malloc(sizeof(struct GDBpTree));

	// Step 02: allocation checking
	if(self == NULL)
		goto error;

	// Step 03: colleciton initialization
	if(GDCollectionInit((struct GDCollection*)self, 0, limitSize, context) != RETCODE_OK)
		goto error;

	// Step 04: initialization other attributes
	self->headNode = NULL;
	if(BpLeafNodeCreate(&self->leafNode, degree, context.contextSize) != RETCODE_OK)
		goto error;

	self->isEmpty = GDBpTreeIsEmpty;
	self->contains = GDBpTreeContains;
	self->add = GDBpTreeAdd;
	self->get = GDBpTreeGet;
	self->remove = GDBpTreeRemove;
	self->removeAll = GDBpTreeRemoveAll;
	self->purge = GDBpTreePurge;
	self->purgeAll = GDBpTreePurgeAll;

	return self;

error:
	return NULL;
}

void GDBpTreeDestroy(struct GDBpTree* self) {
	if(self == NULL)
		return;

	free(self);
}
