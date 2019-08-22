#pragma once

#include "Config.h"
#include "Error.h"
#include "Types.h"


#ifdef __cplusplus
extern "C" {
#endif // _cplusplus

struct MAPI_Graphics;
struct MAPI_RenderTarget;

MEDIA_API MAPI_Graphics* MAPI_Graphics_Create() noexcept;
MEDIA_API void MAPI_Graphics_Destroy(MAPI_Graphics** handle) noexcept;
MEDIA_API void MAPI_Graphics_Initialize(MAPI_Graphics* handle, MAPI_TargetView const* target_view, MAPI_Error* err) noexcept;
MEDIA_API MAPI_RenderTarget* MAPI_Graphics_BeginRendering(MAPI_Graphics* handle, MAPI_Error* err) noexcept;
MEDIA_API void MAPI_Graphic_EndRendering(MAPI_Graphics* handle, MAPI_RenderTarget** target) noexcept;

MEDIA_API void MAPI_RenderTarget_FillColor(MAPI_RenderTarget* handle, MAPI_Color_ARGB const* color, MAPI_Rect const* rect);

#ifdef __cplusplus
}
#endif // _cplusplus
