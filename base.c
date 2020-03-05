#include "base.h"
#include <stdio.h>

#define RETCODE_STR_LEN 32

void	printRetCode(RetCode_t retCode) {
	char strRetCode[RETCODE_STR_LEN] = {};
	switch(retCode) {
		case RETCODE_OK:
			snprintf(strRetCode, RETCODE_STR_LEN, "RETCODE_OK(%d)", retCode);
			break;
		case RETCODE_ERROR:
			snprintf(strRetCode, RETCODE_STR_LEN, "RETCODE_ERROR(%d)", retCode);
			break;
		case RETCODE_EMPTY:
			snprintf(strRetCode, RETCODE_STR_LEN, "RETCODE_EMPTY(%d)", retCode);
			break;
		case RETCODE_INVALID_ARGS:
			snprintf(strRetCode, RETCODE_STR_LEN, "RETCODE_INVALID_ARGS(%d)", retCode);
			break;
		case RETCODE_NOT_ENOUGH_RESOURCES:
			snprintf(strRetCode, RETCODE_STR_LEN, "RETCODE_NOT_ENOUGH_RESOURCES(%d)", retCode);
			break;
		case RETCODE_DENIED_DUE_TO_LIMIT:
			snprintf(strRetCode, RETCODE_STR_LEN, "RETCODE_DENIED_DUE_TO_LIMIT(%d)", retCode);
			break;
	}
	printf("ReturnCode_t is %s\n", strRetCode);
}
