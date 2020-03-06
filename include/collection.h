#ifndef __GD_COLLECTION_H__
#define __GD_COLLECTION_H__

#include "base.h"

#define GD_COLLECTION_API \
	RetCode_t (*isEmtpy)(void* self); \
	RetCode_t (*contains)(void* self, uint64_t element); \
	RetCode_t (*add)(void* self, uint64_t element); \
	RetCode_t (*remove)(void* self, uint64_t element); \
	RetCode_t (*clear)(void* self);

#define GD_COLLECTION_PROPS \
	GD_COLLECTION_API \
	size_t size; \
	size_t limit; \
	struct Context context;

#endif
