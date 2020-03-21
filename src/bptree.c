#include "bptree.h"
#include "stack.h"
#include <stdio.h>

static RetCode_t BpIndexNodeCreate(struct BpIndexNode** node, size_t degree) {
	if(node == NULL)
		return RETCODE_INVALID_ARGS;

	struct BpIndexNode* self = (struct BpIndexNode*)malloc(sizeof(struct BpIndexNode));
	if(self == NULL)
		goto error;

	self->isLeaf = false;
	self->indices = calloc((degree - 1), sizeof(void*));
	self->children = calloc((degree),  sizeof(void*));
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

static void BpIndexNodeDestroy(struct BpIndexNode* node) {
	if(node == NULL)
		return;
	if(node->indices)
		free(node->indices);
	if(node->children)
		free(node->children);
	free(node);
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

static void BpLeafNodeDestroy(struct BpLeafNode* node) {
	if(node == NULL)
		return;
	if(node->values)
		free(node->values);
	free(node);
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

static struct BpLeafNode* GDBpTreeSearch(struct GDBpTree* tree, struct BpIndexNode* node, void* element, struct GDStack* stack) {
	if(tree == NULL || node == NULL || element == NULL)
		return NULL;

	if(stack) {
		stack->push(stack, (void*)node);
	}

	if(node->isLeaf == true)
		return (struct BpLeafNode*)node;

	if(node->indices == NULL)
		return NULL;

	for(int i = 0; i < tree->degree - 1; i++) {
		if(node->indices[i] == NULL)
			return GDBpTreeSearch(tree, node->children[i], element, stack);

		int32_t comp = tree->context.compare(node->indices[i], element);

		// Case 1: current index value is equal to the element
		if(comp ==  0)
			return NULL;
		// Case 2: current index value is greater than the element
		else if(comp > 0)
			return GDBpTreeSearch(tree, node->children[i], element, stack);
	}
	// Case 3: element is the greatest
	return GDBpTreeSearch(tree, node->children[tree->degree - 1], element, stack);
}

static void GDBpTreeSortedInsert(struct GDBpTree* tree, size_t degree, void** arr, void* element) {
	for(size_t i = 0; i < degree; i++) {
		// Case 1: if the selected space is empty
		if(arr[i] == NULL) {
			arr[i] = element;
			return;
		}

		// Case 2: if the element is smaller than selected value
		if(tree->context.compare(arr[i], element) > 0) {
			void* tmp = arr[i];
			arr[i] = element;
			element = tmp;
		}

		// Case 3: if the element is greater than selected value (cont.)
	}
}

static void GDBpTreeSortedInsertIndex(struct GDBpTree* tree, size_t degree, struct BpIndexNode* curNode, struct BpIndexNode* insertingNode) {
	for(size_t i = 0; i < degree; i++) {
		// Case 2-1: if the selected space is empty
		if(curNode->indices[i] == NULL) {
			curNode->indices[i] = insertingNode->indices[0];
			curNode->children[i+1] = insertingNode;
			return;
		}

		// Case 2-2: if the element is smaller than selected value
		if(tree->context.compare(curNode->indices[i], insertingNode->indices[0]) > 0) {
			struct BpIndexNode* tmp = curNode->children[i+1];
			curNode->indices[i] = insertingNode->indices[0];
			curNode->children[i+1] = insertingNode;
			insertingNode = tmp;
		}

		// Case 3: if the element is greater than selected value (cont.)
	}
}

static void GDBpTreeInsertIndex(struct GDBpTree* tree, void* leftNode, void* rightNode, void* index, struct GDStack* stack) {
	if(tree == NULL || stack == NULL)
		return;

	struct BpIndexNode* curNode = NULL;
	stack->pop(stack, &curNode);
	if(curNode == NULL) {
		struct BpIndexNode* headNode;
		BpIndexNodeCreate(&headNode, tree->degree);
		if(headNode == NULL)
			return;
		headNode->indices[0] = index;
		headNode->children[0] = leftNode;
		headNode->children[1] = rightNode;
		tree->headNode = headNode;
		return;
	}

	// Case 1: if the indexNode is full
	if(curNode->indices[tree->degree - 2] != 0) {
		struct BpIndexNode* newNode;
		if(BpIndexNodeCreate(&newNode, tree->degree) != RETCODE_OK)
			return;

		struct BpIndexNode* tmpNode;
		BpIndexNodeCreate(&tmpNode, tree->degree + 1);


		memcpy(tmpNode->indices, curNode->indices, tree->context.contextSize * (tree->degree - 1));
		memcpy(tmpNode->children, curNode->children, sizeof(void*) * tree->degree);
		for(size_t i = 0; i < tree->degree; i++) {
			// Case 2-1: if the selected space is empty
			if(tmpNode->indices[i] == NULL) {
				tmpNode->indices[i] = index;
				tmpNode->children[i+1] = rightNode;
			}

			// Case 2-2: if the element is smaller than selected value
			if(tree->context.compare(tmpNode->indices[i], index) > 0) {
				struct BpIndexNode* tmp = tmpNode->children[i+1];
				tmpNode->indices[i] = index;
				tmpNode->children[i+1] = rightNode;
				rightNode = tmp;
			}

			// Case 3: if the element is greater than selected value (cont.)
		}


		memset(curNode->indices, 0, (tree->degree - 1) * tree->context.contextSize);
		memcpy(curNode->indices, tmpNode->indices, (tree->degree / 2) * tree->context.contextSize);
		memcpy(curNode->children, tmpNode->children, ((tree->degree / 2) + 1) * tree->context.contextSize);
		memcpy(newNode->indices, &tmpNode->indices[(tree->degree / 2) + 1], (tree->degree - (tree->degree / 2) - 1)* tree->context.contextSize);
		memcpy(newNode->children, &tmpNode->children[(tree->degree / 2) + 1], (tree->degree - (tree->degree / 2))* tree->context.contextSize);
		struct BpIndexNode* nextIndex = tmpNode->indices[tree->degree / 2];
		BpIndexNodeDestroy(tmpNode);

		GDBpTreeInsertIndex(tree, curNode, newNode, nextIndex, stack);
	} else {
	// Case 2: indexNode has enough space
		for(size_t i = 0; i < tree->degree - 1 ; i++) {
			// Case 2-1: if the selected space is empty
			if(curNode->indices[i] == NULL) {
				curNode->indices[i] = index;
				curNode->children[i+1] = rightNode;
				return;
			}

			// Case 2-2: if the element is smaller than selected value
			if(tree->context.compare(curNode->indices[i], index) > 0) {
				struct BpIndexNode* tmp = curNode->children[i+1];
				curNode->indices[i] = index;
				curNode->children[i+1] = rightNode;
				rightNode = tmp;
			}

			// Case 3: if the element is greater than selected value (cont.)
		}

	}
}

static void GDBpTreeInsertElement(struct GDBpTree* tree, struct BpLeafNode* node, void* element, struct GDStack* stack) {
	if(tree == NULL || node == NULL || element == NULL)
		return;

	// Case 1: if the leafNode is full
	if(node->values[tree->degree - 2] != 0) {
		struct BpLeafNode* newLeafNode = NULL;
		if(BpLeafNodeCreate(&newLeafNode, tree->degree, tree->context.contextSize) != RETCODE_OK)
			return;

		newLeafNode->next = node->next;
		node->next = newLeafNode;

		struct BpLeafNode* tmpLeafNode;
		BpLeafNodeCreate(&tmpLeafNode, tree->degree + 1, tree->context.contextSize);
		memcpy(tmpLeafNode->values, node->values, tree->context.contextSize * (tree->degree - 1));
		GDBpTreeSortedInsert(tree, tree->degree, tmpLeafNode->values, element);

		memset(node->values, 0, (tree->degree - 1) * tree->context.contextSize);
		memcpy(node->values, tmpLeafNode->values, (tree->degree / 2) * tree->context.contextSize);
		memcpy(newLeafNode->values, &tmpLeafNode->values[tree->degree/2], (tree->degree - (tree->degree/2))* tree->context.contextSize);

		if(stack) {
			struct BpLeafNode* parent;
			stack->pop(stack, &parent);	// will be leafNode
			GDBpLeafDump(tree, parent);

			GDBpTreeInsertIndex(tree, node/*left*/, newLeafNode/*right*/, newLeafNode->values[0]/*index*/, stack);
		}
		BpLeafNodeDestroy(tmpLeafNode);
	} else {
	// Case 2: leafNode has enough space
		GDBpTreeSortedInsert(tree, tree->degree - 1, node->values, element);
	}

	tree->size++;
}

RetCode_t GDBpTreeAdd(void* self, uintptr_t element) {
	struct GDBpTree* tree = (struct GDBpTree*)self;

	if(self == NULL || element == 0)
		return RETCODE_INVALID_ARGS;

	if(tree->size >= tree->capacityLimit)
		return RETCODE_DENIED_DUE_TO_LIMIT;

	struct Context stackContext = {NULL, CONTEXT_TYPE_POINTER, sizeof(void*)};
	struct GDStack* stack = GDStackCreate(10, GD_COLLECTION_SIZE_MAX, stackContext);

	if(stack == NULL)
		return RETCODE_NOT_ENOUGH_RESOURCES;

	struct BpLeafNode* leafNode = GDBpTreeSearch(tree, tree->headNode, (void*)element, stack);
	if(leafNode == NULL)
		goto error;

	GDBpTreeInsertElement(tree, leafNode, element, stack);

	GDStackDestroy(stack);
	return RETCODE_OK;

error:
	if(stack)
		GDStackDestroy(stack);

	return RETCODE_ERROR;
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

static void GDBpIndexDump(struct GDBpTree* tree, int32_t level, struct BpIndexNode* indexNode) {
	if(indexNode == NULL || indexNode->isLeaf)
		return;

	for(int lv = 0; lv < level; lv++)
		printf("\t");

	printf("[%p] ", indexNode);
	for(int i = 0; i < tree->degree - 1; i++) {
		tree->elDump(indexNode->indices[i]);

		if(i != tree->degree -2)
			printf(", ");
	}
	printf("\n");

	for(int i = 0; i < tree->degree; i ++) {
		if(indexNode->children[i])
			GDBpIndexDump(tree, level + 1, indexNode->children[i]);
		else {
			for(int lv = 0; lv < level; lv++)
				printf("\t");
			printf("%p\n", indexNode->children[i]);
		}
	}
}

void GDBpLeafDump(struct GDBpTree* tree, struct BpLeafNode* leafNode) {
	if(leafNode == NULL || !leafNode->isLeaf)
		return;

	printf("\t[%p] ", leafNode);
	for(int i = 0; i < tree->degree - 1; i++) {
		tree->elDump(leafNode->values[i]);

		if(i != tree->degree -2)
			printf(", ");
	}
	printf("\n");

	if(leafNode->next != NULL)
		GDBpLeafDump(tree, leafNode->next);
}

static void GDBpTreeDump(void* self) {
	struct GDBpTree* tree = (struct GDBpTree*)self;
	if(self == NULL)
		return;

	printf("struct GDBpTree(%p) {\n", self);
	printf("\tsize: %ld\n", tree->size);
	printf("\tcapacity: %ld\n", tree->capacity);
	printf("\tcapacityLimit: %ld\n", tree->capacityLimit);
	printf("\tdegree: %ld\n", tree->degree);
	printf("\theadNode: %p\n", tree->headNode);
	printf("\tleafNode: %p\n", tree->leafNode);
	printf("\tcontext: {\n");
	printf("\t\tcontextType: %s\n", tree->context.contextType==CONTEXT_TYPE_POINTER?"CONTEXT_TYPE_POINTER":"CONTEXT_TYPE_VALUE");
	printf("\t\tcontextSize: %ld\n", tree->context.contextSize);
	printf("\t}\n");
	printf("\ttree(HEX, index):\n");
	GDBpIndexDump(tree, 2, tree->headNode);
	printf("\tlinked list(HEX, leaf):\n");
	GDBpLeafDump(tree, tree->leafNode);
	printf("\n");
	printf("}\n");

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
	if(BpLeafNodeCreate(&self->leafNode, degree, context.contextSize) != RETCODE_OK)
		goto error;
	self->headNode = (struct BpIndexNode*)self->leafNode;
	self->degree = degree;

	self->isEmpty = GDBpTreeIsEmpty;
	self->contains = GDBpTreeContains;
	self->add = GDBpTreeAdd;
	self->get = GDBpTreeGet;
	self->remove = GDBpTreeRemove;
	self->removeAll = GDBpTreeRemoveAll;
	self->purge = GDBpTreePurge;
	self->purgeAll = GDBpTreePurgeAll;
	self->dump = GDBpTreeDump;

	return self;

error:
	return NULL;
}

void GDBpTreeDestroy(struct GDBpTree* self) {
	if(self == NULL)
		return;

	free(self);
}
