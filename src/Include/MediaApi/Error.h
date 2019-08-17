#pragma once

#include "Config.h"

enum MAPI_ERROR_CODE
{
	MAPI_NO_ERROR				= 0,
	MAPIERR_FAIL				= -1,	// 无特定含义的一般性失败
	MAPIERR_OUT_OF_MEMORY		= -2,	// 内存不足
	MAPIERR_INVALID_CALL		= -3,	// 错误的调用
};

struct MAPI_Error
{
	MAPI_ERROR_CODE code;
	int32_t			detail;
	const char*		message;
};

