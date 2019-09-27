#pragma once

#include "Config.h"
#include "Error.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif // _cplusplus

MEDIA_API MAPI_Decoder* MAPI_Decoder_Create() noexcept;
MEDIA_API void MAPI_Decoder_Destroy(MAPI_Decoder** obj) noexcept;
MEDIA_API void MAPI_Decoder_Initialize(MAPI_Decoder* obj, 
	MAPI_MediaDescriptor* media, 
	MAPI_Graphics* graphics, 
	MAPI_Error* err) noexcept;
MEDIA_API void MAPI_Decoder_SendPacket(MAPI_Decoder* obj, MAPI_MediaPacket* packet, MAPI_Error* err) noexcept;
MEDIA_API MAPI_MediaFrame* MAPI_Decoder_ReceiveFrame(MAPI_Decoder* obj, MAPI_Error* err) noexcept;

MEDIA_API void MAPI_MediaFrame_Destroy(MAPI_MediaFrame** obj) noexcept;

#ifdef __cplusplus
}
#endif // _cplusplus
