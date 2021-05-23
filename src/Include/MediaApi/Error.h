#pragma once

#include "Config.h"

enum MAPI_ERROR_CODE
{
	MAPI_NO_ERROR				= 0,
	MAPIERR_FAIL				= -1,	// 无特定含义的一般性失败
	MAPIERR_OUT_OF_MEMORY		= -2,	// 内存不足
	MAPIERR_INVALID_CALL		= -3,	// 错误的调用
	MAPIERR_INVALID_ARG			= -4,	// 参数错误
	MAPIERR_NOT_IMPL			= -5,	// 未实现
	MAPIERR_SYSTERM				= -10,	// 系统错误
	MAPIERR_COM					= -11,	// COM 组件返回的错误
	MAPIERR_EndOfStream			= -20,	// 读到流（文件）末尾
};

struct MAPI_Error
{
	MAPI_ERROR_CODE code;
	int32_t			detail;
	const char*		message;
};

