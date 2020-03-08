#ifndef __GJ_QUEUE_H__
#define __GJ_QUEUE_H__

#include "collection.h"

struct GDQueue {
	GD_COLLECTION_PROPS
	RetCode_t	(*push)(void* self, uintptr_t element);
	uintptr_t	(*pop)(void* self);
	void		(*dump)(void* self);
	int32_t		head;
	int32_t		tail;
	void*		queue;
};

struct GDQueue* GDQueueCreate(size_t initialSize, size_t limitSize, struct Context context);
void GDQueueDestroy(struct GDQueue* self);

#endif
