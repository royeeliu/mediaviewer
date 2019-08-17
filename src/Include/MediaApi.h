#pragma once

#include "MediaApi/Config.h"
#include "MediaApi/Error.h"

#ifdef __cplusplus
extern "C" {
#endif // _cplusplus

struct MAPI_MediaSource;

MEDIA_API MAPI_MediaSource* MAPI_MediaSource_Create() noexcept;
MEDIA_API void MAPI_MediaSource_Destroy(MAPI_MediaSource* self) noexcept;
MEDIA_API void MAPI_MediaSource_LoadFile(MAPI_MediaSource* self, const char* u8_url, MAPI_Error* err) noexcept;

#ifdef __cplusplus
}
#endif // _cplusplus

