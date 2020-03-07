#ifndef __GD_COLLECTION_H__
#define __GD_COLLECTION_H__

#include "base.h"

#define GD_COLLECTION_API \
	RetCode_t	(*isEmpty)(void* self); \
	RetCode_t	(*contains)(void* self, uint64_t element); \
	RetCode_t	(*add)(void* self, uint64_t element); \
	uint64_t	(*get)(void* self, uint32_t index); \
	RetCode_t	(*remove)(void* self, uint64_t element); \
	RetCode_t	(*clear)(void* self);

#define GD_COLLECTION_PROPS \
	GD_COLLECTION_API \
	size_t size; \
	size_t capacity; \
	size_t capacityLimit; \
	struct Context context;

struct GDCollection {
	GD_COLLECTION_PROPS
};

struct GDCollection* GDCollectionCreate(size_t initialSize, size_t limitSize, struct Context context);
RetCode_t GDCollectionInit(struct GDCollection* self, size_t initialSize, size_t limitSize, struct Context context);
void GDCollectionDestroy(struct GDCollection* self);

#endif
