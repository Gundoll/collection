#include <stdio.h>
#include "queue.h"

static RetCode_t GDQueuePush(void* self, uintptr_t element) {
	if(self == NULL)
		return RETCODE_INVALID_ARGS;

	struct GDQueue* queue = (struct GDQueue*)self;
	if(queue->size == queue->capacityLimit)
		return RETCODE_DENIED_DUE_TO_LIMIT;

	if(queue->size == queue->capacity) {
		queue->capacity *= 2;
		if(queue->capacity > queue->capacityLimit)
			queue->capacity = queue->capacityLimit;

		queue->queue = realloc(queue->queue, queue->context.contextSize * queue->capacity);
	}

	if(queue->context.contextType == CONTEXT_TYPE_POINTER)
		((uintptr_t*)(queue->queue))[queue->tail] = element;
	else if(queue->context.contextType == CONTEXT_TYPE_VALUE)
		memcpy(queue->queue + (queue->tail * queue->context.contextSize), (void*)element, queue->context.contextSize);

	queue->tail = (queue->tail + 1) % queue->capacityLimit;
	queue->size++;

	return RETCODE_OK;
}

static RetCode_t GDQueuePop(void* self, void* value) {
	if(self == NULL || value == NULL)
		return RETCODE_INVALID_ARGS;

	struct GDQueue* queue = (struct GDQueue*)self;
	if(queue->size <= 0)
		return RETCODE_EMPTY;

	memcpy(value, queue->queue + (queue->head * queue->context.contextSize), queue->context.contextSize);

	queue->head = (queue->head + 1) % queue->capacityLimit;
	queue->size--;

	return RETCODE_OK;
}

static RetCode_t GDQueueIsEmpty(void* self) {
	struct GDQueue* queue = (struct GDQueue*)self;
	if(queue == NULL)
		return RETCODE_INVALID_ARGS;
	if(queue->size > 0)
		return RETCODE_ERROR;
	if(queue->size == 0)
		return RETCODE_OK;
}

static RetCode_t GDQueueContains(void* self, uintptr_t element) {
	if(self == NULL)
		return RETCODE_INVALID_ARGS;

	struct GDQueue* queue = (struct GDQueue*)self;
	if(queue->size == 0)
		return RETCODE_EMPTY;

	uint32_t index = queue->head;
	for(int cnt = 0; cnt < queue->size; cnt++) {
		if(queue->context.contextType == CONTEXT_TYPE_POINTER && queue->context.compare(queue->queue + (index * queue->context.contextSize), (void*)element) == 0)
			return RETCODE_OK;
		else if(queue->context.contextType == CONTEXT_TYPE_VALUE && queue->context.compare(queue->queue + (index * queue->context.contextSize), (void*)element) == 0)
			return RETCODE_OK;
		else
			index = (index + 1) % queue->capacityLimit;
	}

	return RETCODE_ERROR;
}

static RetCode_t GDQueueAdd(void* self, uintptr_t element) {
	return GDQueuePush(self, element);
}

static RetCode_t GDQueueGet(void* self, uint32_t index, void* value) {
	return 0;
}

static RetCode_t GDQueueRemove(void* self, uintptr_t element) {
	return RETCODE_UNSUPPORTED_FUNCTION;
}

static RetCode_t GDQueueRemoveAll(void* self) {
	return RETCODE_OK;
}

static RetCode_t GDQueuePurge(void* self, uintptr_t element) {
	return RETCODE_OK;
}

static RetCode_t GDQueuePurgeAll(void* self) {
	return RETCODE_OK;
}

static void GDQueueDump(void* self) {
	struct GDQueue* queue = (struct GDQueue*)self;
	if(self == NULL)
		return;

	printf("struct GDQueue(%p) {\n", self);
	printf("\tsize: %ld\n", queue->size);
	printf("\tcapacity: %ld\n", queue->capacity);
	printf("\tcapacityLimit: %ld\n", queue->capacityLimit);
	printf("\thead: %d\n", queue->head);
	printf("\ttail: %d\n", queue->tail);
	printf("\tcontext: {\n");
	printf("\t\tcontextType: %s\n", queue->context.contextType==CONTEXT_TYPE_POINTER?"CONTEXT_TYPE_POINTER":"CONTEXT_TYPE_VALUE");
	printf("\t\tcontextSize: %ld\n", queue->context.contextSize);
	printf("\t}\n");
	printf("\tqueue(HEX):");
	for(int i = 0; i < queue->capacity * queue->context.contextSize; i++) {
		if(i % 8 == 0)
			printf("\n\t[%04X]  ", i);
		else if(i > 0 && i % 4 == 0)
			printf(" ");

		printf("%02X ", ((uint8_t*)queue->queue)[i]);
	}
	printf("\n");
	printf("}\n");

}

struct GDQueue* GDQueueCreate(size_t initialSize, size_t limitSize, struct Context context) {
	// Step 01: memory allocation
	struct GDQueue* self = (struct GDQueue*)malloc(sizeof(struct GDQueue));

	// Step 02: allocation checking
	if(self == NULL)
		goto error;

	// Step 03: colleciton initialization
	if(GDCollectionInit((struct GDCollection*)self, initialSize, limitSize, context) != RETCODE_OK)
		goto error;

	// Step 04: queue capacity preparation
	if(initialSize > 0) {
		self->queue = malloc(context.contextSize * initialSize);
		if(self->queue == NULL)
			goto error;
	}

	self->head = 0;
	self->tail = 0;

	self->isEmpty = GDQueueIsEmpty;
	self->contains = GDQueueContains;
	self->add = GDQueueAdd;
	self->get = GDQueueGet;
	self->remove = GDQueueRemove;
	self->removeAll = GDQueueRemoveAll;
	self->purge = GDQueuePurge;
	self->purgeAll = GDQueuePurgeAll;
	self->push = GDQueuePush;
	self->pop = GDQueuePop;
	self->dump = GDQueueDump;

	return self;

error:
	if(self) {
		if(self->queue)
			free(self->queue);
		free(self);
	}
	return NULL;
}

void GDQueueDestroy(struct GDQueue* self) {
	if(self == NULL)
		return;
	if(self->queue)
		free(self->queue);
	free(self);
}
