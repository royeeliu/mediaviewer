#pragma once

#include "Config.h"
#include "Error.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif // _cplusplus

MEDIA_API MAPI_VideoRenderer* MAPI_VideoRenderer_Create() noexcept;
MEDIA_API void MAPI_VideoRenderer_Destroy(MAPI_VideoRenderer** obj) noexcept;
MEDIA_API void MAPI_VideoRenderer_Initialize(
	MAPI_VideoRenderer* obj, 
	MAPI_MediaDescriptor* media, 
	MAPI_Graphics* graphics, 
	MAPI_Error* err) noexcept;
MEDIA_API void MAPI_VideoRenderer_Repaint(MAPI_VideoRenderer* obj) noexcept;
MEDIA_API void MAPI_VideoRenderer_SendFrame(
	MAPI_VideoRenderer* obj,
	MAPI_MediaFrame* frame) noexcept;
MEDIA_API void MAPI_VideoRenderer_SetBackgroundColor(
	MAPI_VideoRenderer* obj, 
	MAPI_Color_ARGB const* color) noexcept;

#ifdef __cplusplus
}
#endif // _cplusplus
