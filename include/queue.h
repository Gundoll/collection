#ifndef __GJ_QUEUE_H__
#define __GJ_QUEUE_H__

#include "collection.h"

struct GDQueue {
	GD_COLLECTION_PROPS
	void* queue;
};

struct GDQueue* GDQueueCreate(size_t initialSize, size_t limitSize, struct Context context);
void GDQueueDestroy(struct GDQueue* self);

#endif
