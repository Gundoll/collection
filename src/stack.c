#include <stdio.h>
#include "stack.h"

static RetCode_t GDPush(void* self, uintptr_t element) {
	if(self == NULL)
		return RETCODE_INVALID_ARGS;

	struct GDStack* stack = (struct GDStack*)self;
	if(stack->size == stack->capacityLimit)
		return RETCODE_DENIED_DUE_TO_LIMIT;

	if(stack->size == stack->capacity) {
		stack->capacity *= 2;
		if(stack->capacity > stack->capacityLimit)
			stack->capacity = stack->capacityLimit;

		stack->stack = realloc(stack->stack, stack->context.contextSize * stack->capacity);
	}

	if(stack->context.contextType == CONTEXT_TYPE_POINTER)
		((uintptr_t*)(stack->stack))[stack->size] = element;
	else if(stack->context.contextType == CONTEXT_TYPE_VALUE)
		memcpy(stack->stack + (stack->size * stack->context.contextSize), (void*)element, stack->context.contextSize);

	stack->size++;

	return RETCODE_OK;
}

static RetCode_t GDPop(void* self, void* value) {
	if(self == NULL || value == NULL)
		return RETCODE_INVALID_ARGS;

	struct GDStack* stack = (struct GDStack*)self;
	if(stack->size <= 0)
		return RETCODE_EMPTY;

	memcpy(value, stack->stack + ((stack->size - 1) * stack->context.contextSize), stack->context.contextSize);

	stack->size--;

	return RETCODE_OK;
}

static RetCode_t GDPeek(void* self, void* value) {
	if(self == NULL || value == NULL)
		return RETCODE_INVALID_ARGS;

	struct GDStack* stack = (struct GDStack*)self;
	if(stack->size <= 0)
		return RETCODE_EMPTY;

	memcpy(value, stack->stack + ((stack->size - 1) * stack->context.contextSize), stack->context.contextSize);

	return RETCODE_OK;
}

static RetCode_t GDIsEmpty(void* self) {
	struct GDStack* stack = (struct GDStack*)self;
	if(stack == NULL)
		return RETCODE_INVALID_ARGS;
	if(stack->size > 0)
		return RETCODE_ERROR;
	if(stack->size == 0)
		return RETCODE_OK;
}

static RetCode_t GDContains(void* self, uintptr_t element) {
	if(self == NULL)
		return RETCODE_INVALID_ARGS;

	struct GDStack* stack = (struct GDStack*)self;
	if(stack->size == 0)
		return RETCODE_EMPTY;

	for(int cnt = 0; cnt < stack->size; cnt++) {
		if(stack->context.contextType == CONTEXT_TYPE_POINTER && stack->context.compare(stack->stack + (cnt * stack->context.contextSize), (void*)element) == 0)
			return RETCODE_OK;
		else if(stack->context.contextType == CONTEXT_TYPE_VALUE && stack->context.compare(stack->stack + (cnt * stack->context.contextSize), (void*)element) == 0)
			return RETCODE_OK;
	}

	return RETCODE_ERROR;
}

static RetCode_t GDAdd(void* self, uintptr_t element) {
	return GDPush(self, element);
}

static RetCode_t GDGet(void* self, uint32_t index, void* value) {
	return RETCODE_UNSUPPORTED_FUNCTION;
}

static RetCode_t GDRemove(void* self, uintptr_t element) {
	return RETCODE_UNSUPPORTED_FUNCTION;
}

static RetCode_t GDRemoveAll(void* self) {
	struct GDStack* stack = (struct GDStack*)self;

	if(self == NULL) {
		return RETCODE_INVALID_ARGS;
	}

	stack->size = 0;

	return RETCODE_OK;
}

static RetCode_t GDPurge(void* self, uintptr_t element) {
	return RETCODE_UNSUPPORTED_FUNCTION;
}

static RetCode_t GDPurgeAll(void* self) {
	printf("%s\n", __FUNCTION__);
	struct GDStack* stack = (struct GDStack*)self;

	if(self == NULL) {
		return RETCODE_INVALID_ARGS;
	}

	if(stack->context.contextType == CONTEXT_TYPE_POINTER) {
		int i = 0;
		for(i = 0; i < stack->size; i++) {
			void** ptr = stack->stack + (i * stack->context.contextSize);
			if(ptr)
				free(*ptr);
		}
	}

	stack->size = 0;

	return RETCODE_OK;
}

static void GDDump(void* self) {
	struct GDStack* stack = (struct GDStack*)self;
	if(self == NULL)
		return;

	printf("struct GDStack(%p) {\n", self);
	printf("\tsize: %ld\n", stack->size);
	printf("\tcapacity: %ld\n", stack->capacity);
	printf("\tcapacityLimit: %ld\n", stack->capacityLimit);
	printf("\tcontext: {\n");
	printf("\t\tcontextType: %s\n", stack->context.contextType==CONTEXT_TYPE_POINTER?"CONTEXT_TYPE_POINTER":"CONTEXT_TYPE_VALUE");
	printf("\t\tcontextSize: %ld\n", stack->context.contextSize);
	printf("\t}\n");
	printf("\tstack(HEX):");
	for(int i = 0; i < stack->capacity * stack->context.contextSize; i++) {
		if(i % 8 == 0)
			printf("\n\t[%04X]  ", i);
		else if(i > 0 && i % 4 == 0)
			printf(" ");

		printf("%02X ", ((uint8_t*)stack->stack)[i]);
	}
	printf("\n");
	printf("}\n");

}

struct GDStack* GDStackCreate(size_t initialSize, size_t limitSize, struct Context context) {
	// Step 01: memory allocation
	struct GDStack* self = (struct GDStack*)malloc(sizeof(struct GDStack));

	// Step 02: allocation checking
	if(self == NULL)
		goto error;

	// Step 03: colleciton initialization
	if(GDCollectionInit((struct GDCollection*)self, initialSize, limitSize, context) != RETCODE_OK)
		goto error;

	// Step 04: stack capacity preparation
	if(initialSize > 0) {
		self->stack = malloc(context.contextSize * initialSize);
		if(self->stack == NULL)
			goto error;
	}

	self->isEmpty = GDIsEmpty;
	self->contains = GDContains;
	self->add = GDAdd;
	self->get = GDGet;
	self->remove = GDRemove;
	self->removeAll = GDRemoveAll;
	self->purge = GDPurge;
	self->purgeAll = GDPurgeAll;
	self->push = GDPush;
	self->pop = GDPop;
	self->peek = GDPeek;
	self->dump = GDDump;

	return self;

error:
	if(self) {
		if(self->stack)
			free(self->stack);
		free(self);
	}
	return NULL;
}

void GDStackDestroy(struct GDStack* self) {
	if(self == NULL)
		return;
	if(self->stack)
		free(self->stack);
	free(self);
}
