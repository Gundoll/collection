#include <stdio.h>
#include "collection.h"

struct GDCollection* GDCollectionCreate(size_t initialSize, size_t limitSize, struct Context context) {
	// Step 01: memory allocation
	struct GDCollection* self = (struct GDCollection*)malloc(sizeof(struct GDCollection));

	// Step 02: allocaiton result checking
	if(self == NULL)
		goto error;

	// Step 03: initialization
	if(GDCollectionInit(self, initialSize, limitSize, context) != RETCODE_OK)
		goto error;

	return self;

error:
	if(self)
		free(self);

	return NULL;
}

RetCode_t GDCollectionInit(struct GDCollection* self, size_t initialSize, size_t limitSize, struct Context context) {
	RetCode_t ret = RETCODE_ERROR;

	// Step 01: argument checking
	if(self == NULL) {
		ret = RETCODE_INVALID_ARGS;
		goto error;
	}

	// Step 02: context checking
	if(context.contextSize <= 0) {
		ret = RETCODE_INVALID_ARGS;
		goto error;
	}

	// Step 03: attribute setting
	self->size = 0;
	self->capacity = initialSize;
	self->capacityLimit = limitSize;
	self->context = context;

	return RETCODE_OK;

error:
	printf("[ERROR (%02d)] %s in %s", ret, __FUNCTION__, __FILE__);
	return ret;
}

void GDCollectionDestroy(struct GDCollection* self) {
	if(self)
		free(self);
}
