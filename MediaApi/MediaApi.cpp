#include "pch.h"
#include "Include/media_api.h"

struct MAPI_FormatContext
{
};

MEDIA_API MAPI_FormatContext* MAPI_Format_AllocContext(MAPI_ErrorCode* ec)
{
	return new MAPI_FormatContext{};
}

MEDIA_API void MAPI_Format_FreeContext(MAPI_FormatContext* ctx)
{
	delete ctx;
	return;
}
