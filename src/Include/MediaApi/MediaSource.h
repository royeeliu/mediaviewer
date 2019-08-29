#pragma once

#include "Config.h"
#include "Error.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif // _cplusplus

struct MAPI_MediaSource;
struct MAPI_Presenter;
struct MAPI_SourceStream;

MEDIA_API MAPI_MediaSource* MAPI_MediaSource_Create() noexcept;
MEDIA_API void MAPI_MediaSource_Destroy(MAPI_MediaSource** obj) noexcept;
MEDIA_API void MAPI_MediaSource_LoadFile(MAPI_MediaSource* obj, const char* u8_url, MAPI_Error* err) noexcept;

MEDIA_API uint32_t MAPI_MediaSource_GetStreamCount(MAPI_MediaSource* obj) noexcept;
MEDIA_API MAPI_SourceStream* MAPI_MediaSource_GetStream(MAPI_MediaSource* obj, uint32_t index) noexcept;

MEDIA_API void MAPI_SourceStream_Destroy(MAPI_SourceStream** obj) noexcept;
MEDIA_API MAPI_MediaType MAPI_SourceStream_GetMediaType(MAPI_SourceStream* obj) noexcept;
MEDIA_API uint32_t MAPI_SourceStream_GetId(MAPI_SourceStream* obj) noexcept;

#ifdef __cplusplus
}
#endif // _cplusplus
