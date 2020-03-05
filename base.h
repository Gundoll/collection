#ifndef __GD_BASE_H__
#define __GD_BASE_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
	RETCODE_OK,
	RETCODE_ERROR,
	RETCODE_EMPTY,
	RETCODE_INVALID_ARGS,
	RETCODE_NOT_ENOUGH_RESOURCES,
	RETCODE_DENIED_DUE_TO_LIMIT,
} RetCode_t;

struct Context {
	int32_t (*compare)(void* lValue, void* rValue);
	size_t contextSize;
};

void	printRetCode(RetCode_t retCode);

#endif
