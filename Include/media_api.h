#pragma once

#include "apis/media_api_config.h"

#ifdef _cplusplus
extern "C" {
#endif // _cplusplus

typedef struct MAPI_FormatContext MAPI_FormatContext;

typedef struct
{
	int32_t code;
} MAPI_ErrorCode;

MEDIA_API MAPI_FormatContext* MAPI_Format_AllocContext(MAPI_ErrorCode* ec);
MEDIA_API void MAPI_Format_FreeContext(MAPI_FormatContext* ctx);

#ifdef _cplusplus
}
#endif // _cplusplus

