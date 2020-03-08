#include <stdio.h>
#include "queue.h"

static RetCode_t push(void* self, uint64_t element) {
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
		memcpy(queue->queue + (queue->tail * queue->context.contextSize), element, queue->context.contextSize);

	queue->tail = (queue->tail + 1) % queue->capacityLimit;
	queue->size++;

	return RETCODE_OK;
}

static uint64_t pop(void* self) {
}

static RetCode_t isEmpty(void* self) {
	struct GDQueue* queue = (struct GDQueue*)self;
	if(queue == NULL)
		return RETCODE_INVALID_ARGS;
	if(queue->size > 0)
		return RETCODE_ERROR;
	if(queue->size == 0)
		return RETCODE_OK;
}

static RetCode_t contains(void* self, uint64_t element) {
	if(self == NULL)
		return RETCODE_INVALID_ARGS;

	struct GDQueue* queue = (struct GDQueue*)self;
	if(queue->size == 0)
		return RETCODE_EMPTY;

	uint32_t index = queue->head;
	for(int cnt = 0; cnt < queue->size; cnt++) {
		if(queue->context.contextType == CONTEXT_TYPE_POINTER && queue->context.compare(queue->queue + (index * queue->context.contextSize), element) == 0)
			return RETCODE_OK;
		else if(queue->context.contextType == CONTEXT_TYPE_VALUE && queue->context.compare(queue->queue + (index * queue->context.contextSize), element) == 0)
			return RETCODE_OK;
		else
			index = (index + 1) % queue->capacityLimit;
	}

	return RETCODE_ERROR;
}

static RetCode_t add(void* self, uint64_t element) {
	return push(self, element);
}

static uint64_t	get(void* self, uint32_t index) {
	return NULL;
}

static RetCode_t queueRemove(void* self, uint64_t element) {
	return RETCODE_UNSUPPORTED_FUNCTION;
}

static RetCode_t clear(void* self) {
	return RETCODE_OK;
}

static void dump(void* self) {
	struct GDQueue* queue = (struct GDQueue*)self;
	if(self == NULL)
		return;

	printf("struct GDQueue {\n");
	printf("\tsize: %ld", queue->size);
	printf("\tcapacity: %ld", queue->capacity);
	printf("\tcapacityLimit: %ld", queue->capacityLimit);
	printf("\thead: %d\n", queue->head);
	printf("\ttail: %d\n", queue->tail);
	printf("\tcontext: {\n");
	printf("\t\tcontextType: %s\n", queue->context.contextType==CONTEXT_TYPE_POINTER?"CONTEXT_TYPE_POINTER":"CONTEXT_TYPE_VALUE");
	printf("\t\tcontextSize: %ld\n", queue->context.contextSize);
	printf("\t}\n");
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

	self->isEmpty = isEmpty;
	self->contains = contains;
	self->add = add;
	self->get = get;
	self->remove = queueRemove;
	self->clear = clear;
	self->push = push;
	self->pop = pop;
	self->dump = dump;

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
