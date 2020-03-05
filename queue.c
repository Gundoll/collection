#include "queue.h"

struct GDQueue* GDQueueCreate(size_t initialSize, size_t limitSize, struct Context context) {
	if(context.contextSize <= 0)
		goto error;

	struct GDQueue* self = (struct GDQueue*)malloc(sizeof(struct GDQueue));
	if(self == NULL)
		goto error;

	self->context = context;

	if(initialSize > 0) {
		self->queue = malloc(context.contextSize * initialSize);
		if(self->queue == NULL)
			goto error;
	}

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
