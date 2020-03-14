#ifndef __GD_STACK_H___
#define __GD_STACK_H__

#include "collection.h"

struct GDStack {
	GD_COLLECTION_PROPS
	RetCode_t	(*push)(void* self, uintptr_t element);
	RetCode_t	(*pop)(void* self, void* value);
	RetCode_t	(*peek)(void* self, void* value);
	void		(*dump)(void* self);
	void*		stack;
};

struct GDStack* GDStackCreate(size_t initialSize, size_t limitSize, struct Context context);
void GDStackDestroy(struct GDStack* self);

#endif
