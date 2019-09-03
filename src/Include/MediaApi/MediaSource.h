#pragma once

#include "Config.h"
#include "Error.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif // _cplusplus


MEDIA_API MAPI_MediaSource* MAPI_MediaSource_Create() noexcept;
MEDIA_API void MAPI_MediaSource_Destroy(MAPI_MediaSource** obj) noexcept;
MEDIA_API void MAPI_MediaSource_LoadFile(MAPI_MediaSource* obj, const char* u8_url, MAPI_Error* err) noexcept;
MEDIA_API uint32_t MAPI_MediaSource_GetStreamCount(MAPI_MediaSource* obj) noexcept;
MEDIA_API int32_t MAPI_MediaSource_FindBestStream(MAPI_MediaSource* obj, MAPI_MediaType type) noexcept;
MEDIA_API MAPI_StreamDescriptor* MAPI_MediaSource_GetStreamDescriptor(MAPI_MediaSource* obj, uint32_t index) noexcept;
MEDIA_API MAPI_MediaPacket* MAPI_MediaSource_ReadPacket(MAPI_MediaSource* obj, MAPI_Error* err);

MEDIA_API void MAPI_StreamDescriptor_Destroy(MAPI_StreamDescriptor** obj) noexcept;
MEDIA_API MAPI_MediaType MAPI_StreamDescriptor_GetMediaType(MAPI_StreamDescriptor* obj) noexcept;
MEDIA_API uint32_t MAPI_StreamDescriptor_GetId(MAPI_StreamDescriptor* obj) noexcept;
MEDIA_API void MAPI_StreamDescriptor_GetTimebase(MAPI_StreamDescriptor* obj, MAPI_Rational* timebase) noexcept;
MEDIA_API MAPI_MediaAttributes* MAPI_StreamDescriptor_GetMediaAttributes(MAPI_StreamDescriptor* obj) noexcept;

MEDIA_API void MAPI_MediaAttributes_Destroy(MAPI_MediaAttributes** obj) noexcept;

MEDIA_API void MAPI_MediaPacket_Destroy(MAPI_MediaPacket** obj) noexcept;
MEDIA_API uint32_t MAPI_MediaPacket_GetStreamIndex(MAPI_MediaPacket* obj) noexcept;
MEDIA_API int64_t MAPI_MediaPacket_GetPts(MAPI_MediaPacket* obj) noexcept;
MEDIA_API int64_t MAPI_MediaPacket_GetDts(MAPI_MediaPacket* obj) noexcept;
MEDIA_API void MAPI_MediaPacket_GetTimebase(MAPI_MediaPacket* obj, MAPI_Rational* timebase) noexcept;

#ifdef __cplusplus
}
#endif // _cplusplus
