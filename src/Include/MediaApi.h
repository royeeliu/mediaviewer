#pragma once

#include "MediaApi/Config.h"
#include "MediaApi/Error.h"
#include "MediaApi/Types.h"
#include "MediaApi/Graphics.h"

#ifdef __cplusplus
extern "C" {
#endif // _cplusplus

struct MAPI_MediaSource;
struct MAPI_Presenter;

MEDIA_API MAPI_MediaSource* MAPI_MediaSource_Create() noexcept;
MEDIA_API void MAPI_MediaSource_Destroy(MAPI_MediaSource* self) noexcept;
MEDIA_API void MAPI_MediaSource_LoadFile(MAPI_MediaSource* self, const char* u8_url, MAPI_Error* err) noexcept;

MEDIA_API MAPI_Presenter* MAPI_Presenter_Create() noexcept;
MEDIA_API void MAPI_Presenter_Destroy(MAPI_Presenter* self) noexcept;
MEDIA_API void MAPI_Presenter_Initialize(MAPI_Presenter* self, MAPI_TargetView const* view, MAPI_Error* err) noexcept;

#ifdef __cplusplus
}
#endif // _cplusplus

