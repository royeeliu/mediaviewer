#pragma once

#include "MediaApi/Graphics.h"
#include "MediaApi/MediaSource.h"

#ifdef __cplusplus
extern "C" {
#endif // _cplusplus


MEDIA_API MAPI_Presenter* MAPI_Presenter_Create() noexcept;
MEDIA_API void MAPI_Presenter_Destroy(MAPI_Presenter* obj) noexcept;
MEDIA_API void MAPI_Presenter_Initialize(MAPI_Presenter* obj, MAPI_TargetView const* view, MAPI_Error* err) noexcept;

#ifdef __cplusplus
}
#endif // _cplusplus

