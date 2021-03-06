#include "pch.h"
#include "Include/MediaApi.h"
#include "MediaSource.h"
#include "MediaPacket.h"
#include "MediaFrame.h"
#include "FFmpegMediaDescriptor.h"
#include "Presenter.h"
#include "TargetView.h"
#include "Graphics.h"
#include "RenderTarget.h"
#include "Decoder.h"
#include "VideoRenderer.h"

using namespace mapi;

#define PRINTF printf

template<class T>
void SafeDelete(T** pptr)
{
	if (pptr != nullptr)
	{
		T* ptr = *pptr;
		*pptr = nullptr;
		delete ptr;
	}
}

template<typename T>
struct MediaStruct
{
	using Class = typename T;
	Class impl;
};

template<typename T>
struct MediaStruct<T*>
{
	using Class = typename T;
	T* impl = nullptr;
};

struct MAPI_MediaSource : MediaStruct<MediaSource> {};
struct MAPI_Decoder : MediaStruct<Decoder>{};
struct MAPI_Presenter : MediaStruct<Presenter> {};
struct MAPI_Graphics : MediaStruct<Graphics> {};
struct MAPI_RenderTarget : MediaStruct<RenderTarget*> {};
struct MAPI_StreamDescriptor : MediaStruct<StreamDescriptor*> {};
struct MAPI_MediaDescriptor : MediaStruct<std::shared_ptr<MediaDescriptor>>{};
struct MAPI_MediaPacket : MediaStruct<std::shared_ptr<MediaPacket>> {};
struct MAPI_MediaFrame : MediaStruct<std::shared_ptr<MediaFrame>> {};
struct MAPI_VideoRenderer : MediaStruct<VideoRenderer> {};


MEDIA_API MAPI_MediaSource* MAPI_MediaSource_Create() noexcept
{
	return new MAPI_MediaSource{};
}

MEDIA_API void MAPI_MediaSource_Destroy(MAPI_MediaSource** obj) noexcept
{
	SafeDelete(obj);
}

MEDIA_API void MAPI_MediaSource_LoadFile(MAPI_MediaSource* obj, const char* u8_url, MAPI_Error* err) noexcept
{
	*err = { MAPI_NO_ERROR };
	obj->impl.LoadFile(u8_url, *err);
}

MEDIA_API uint32_t MAPI_MediaSource_GetStreamCount(MAPI_MediaSource* obj) noexcept
{
	return obj->impl.GetStreamCount();
}

MEDIA_API int32_t MAPI_MediaSource_FindBestStream(MAPI_MediaSource* obj, MAPI_MediaType type) noexcept
{
	return obj->impl.FindBestStream(type);
}

MEDIA_API MAPI_StreamDescriptor* MAPI_MediaSource_GetStreamDescriptor(MAPI_MediaSource* obj, uint32_t index) noexcept
{
	auto stream = new MAPI_StreamDescriptor{};
	stream->impl = obj->impl.GetStreamDescriptor(index);
	return stream;
}

MEDIA_API MAPI_MediaPacket* MAPI_MediaSource_ReadPacket(MAPI_MediaSource* obj, MAPI_Error* err)
{
	*err = { MAPI_NO_ERROR };
	auto packet = obj->impl.ReadNextPacket(*err);

	if (err->code != MAPI_NO_ERROR)
	{
		return nullptr;
	}

	MAPI_MediaPacket* result = new MAPI_MediaPacket{};
	result->impl = std::move(packet);
	return result;
}

MEDIA_API void MAPI_StreamDescriptor_Destroy(MAPI_StreamDescriptor** obj) noexcept
{
	SafeDelete(obj);
}

MEDIA_API MAPI_MediaType MAPI_StreamDescriptor_GetMediaType(MAPI_StreamDescriptor* obj) noexcept
{
	return obj->impl->GetMediaType();
}

MEDIA_API uint32_t MAPI_StreamDescriptor_GetId(MAPI_StreamDescriptor* obj) noexcept
{
	return obj->impl->GetId();
}

MEDIA_API void MAPI_StreamDescriptor_GetTimebase(MAPI_StreamDescriptor* obj, MAPI_Rational* timebase) noexcept
{
	*timebase = obj->impl->GetTimebase();
}

MEDIA_API MAPI_MediaDescriptor* MAPI_StreamDescriptor_GetMediaDescriptor(MAPI_StreamDescriptor* obj) noexcept
{
	auto ptr = new MAPI_MediaDescriptor{};
	ptr->impl = std::move(obj->impl->GetMediaDescriptor());
	return ptr;
}

MEDIA_API void MAPI_MediaDescriptor_Destroy(MAPI_MediaDescriptor** obj) noexcept
{
	SafeDelete(obj);
}

MEDIA_API void MAPI_MediaPacket_Destroy(MAPI_MediaPacket** obj) noexcept
{
	SafeDelete(obj);
}

MEDIA_API uint32_t MAPI_MediaPacket_GetStreamIndex(MAPI_MediaPacket* obj) noexcept
{
	return obj->impl->GetStreamIndex();
}

MEDIA_API int64_t MAPI_MediaPacket_GetPts(MAPI_MediaPacket* obj) noexcept
{
	return obj->impl->GetPts();
}

MEDIA_API int64_t MAPI_MediaPacket_GetDts(MAPI_MediaPacket* obj) noexcept
{
	return obj->impl->GetDts();
}

MEDIA_API void MAPI_MediaPacket_GetTimebase(MAPI_MediaPacket* obj, MAPI_Rational* timebase) noexcept
{
	*timebase = obj->impl->GetTimebase();
}

MEDIA_API MAPI_Presenter* MAPI_Presenter_Create() noexcept
{
	return new MAPI_Presenter{};
}

MEDIA_API void MAPI_Presenter_Destroy(MAPI_Presenter* obj) noexcept
{
	SafeDelete(&obj);
}

MEDIA_API void MAPI_Presenter_Initialize(MAPI_Presenter* obj, MAPI_TargetView const* view, MAPI_Error* err) noexcept
{
	*err = { MAPI_NO_ERROR };
	obj->impl.Initialize(std::make_unique<TargetView>(view), *err);
}

MEDIA_API MAPI_Graphics* MAPI_Graphics_Create() noexcept
{
	return new MAPI_Graphics{};
}

MEDIA_API void MAPI_Graphics_Destroy(MAPI_Graphics** phandle) noexcept
{
	SafeDelete(phandle);
}

MEDIA_API void MAPI_Graphics_Initialize(MAPI_Graphics* handle, MAPI_TargetView const* target_view, MAPI_Error* err) noexcept
{
	*err = { MAPI_NO_ERROR };
	handle->impl.Initialize(std::make_unique<TargetView>(target_view), *err);
}

MEDIA_API MAPI_RenderTarget* MAPI_Graphics_BeginRender(MAPI_Graphics* handle, MAPI_Error* err) noexcept
{
	*err = { MAPI_NO_ERROR };
	auto impl = handle->impl.BeginRendering(*err);

	if (err->code != MAPI_NO_ERROR)
	{
		return nullptr;
	}

	auto result = new MAPI_RenderTarget{};
	std::swap(result->impl, impl);

	return result;
}

MEDIA_API void MAPI_Graphic_EndRender(MAPI_Graphics* handle, MAPI_RenderTarget** target, MAPI_Error* err) noexcept
{
	*err = { MAPI_NO_ERROR };
	handle->impl.EndRendering((*target)->impl, *err);
	SafeDelete(target);
}

MEDIA_API void MAPI_RenderTarget_FillColor(MAPI_RenderTarget* handle, MAPI_Color_ARGB const* color, MAPI_Rect const* rect)
{
	handle->impl->FillColor(*color, rect);
}

MEDIA_API MAPI_Decoder* MAPI_Decoder_Create() noexcept
{
	return new MAPI_Decoder{};
}

MEDIA_API void MAPI_Decoder_Destroy(MAPI_Decoder** obj) noexcept
{
	SafeDelete(obj);
}

MEDIA_API void MAPI_Decoder_Initialize(MAPI_Decoder* obj, MAPI_MediaDescriptor* media, MAPI_Graphics* graphics, MAPI_Error* err) noexcept
{
	*err = { MAPI_NO_ERROR };
	Graphics* graphParam = (graphics != nullptr ? &graphics->impl : nullptr);
	obj->impl.Initialize(*(media->impl), graphParam, *err);
}

MEDIA_API void MAPI_Decoder_SendPacket(MAPI_Decoder* obj, MAPI_MediaPacket* packet, MAPI_Error* err) noexcept
{
	*err = { MAPI_NO_ERROR };
	obj->impl.SendPacket(*(packet->impl), *err);
}

MEDIA_API MAPI_MediaFrame* MAPI_Decoder_ReceiveFrame(MAPI_Decoder* obj, MAPI_Error* err) noexcept
{
	*err = { MAPI_NO_ERROR };
	std::shared_ptr<MediaFrame> frame = obj->impl.ReceiveFrame(*err);

	if (frame != nullptr)
	{
		_ASSERT(err->code == MAPI_NO_ERROR);
		MAPI_MediaFrame* result = new MAPI_MediaFrame{};
		result->impl = std::move(frame);
		return result;
	}

	return nullptr;
}

MEDIA_API MAPI_MediaDescriptor* MAPI_Decoder_GetOutputMediaDescriptor(MAPI_Decoder* obj) noexcept
{
	auto desc = new MAPI_MediaDescriptor{};
	desc->impl = obj->impl.GetMediaDescriptor();
	return desc;
}

MEDIA_API void MAPI_MediaFrame_Destroy(MAPI_MediaFrame** obj) noexcept
{
	SafeDelete(obj);
}

MEDIA_API MAPI_VideoRenderer* MAPI_VideoRenderer_Create() noexcept
{
	return new MAPI_VideoRenderer{};
}

MEDIA_API void MAPI_VideoRenderer_Destroy(MAPI_VideoRenderer** obj) noexcept
{
	SafeDelete(obj);
}

MEDIA_API void MAPI_VideoRenderer_Initialize(MAPI_VideoRenderer* obj, MAPI_MediaDescriptor* media, MAPI_Graphics* graphics, MAPI_Error* err) noexcept
{
	*err = { MAPI_NO_ERROR };
	obj->impl.Initialize(*(media->impl), graphics->impl, *err);
}

MEDIA_API void MAPI_VideoRenderer_Repaint(MAPI_VideoRenderer* obj) noexcept
{
	obj->impl.Repaint();
}

MEDIA_API void MAPI_VideoRenderer_SendFrame(MAPI_VideoRenderer* obj, MAPI_MediaFrame* frame) noexcept
{
	obj->impl.Present(frame->impl);
}

MEDIA_API void MAPI_VideoRenderer_SetBackgroundColor(MAPI_VideoRenderer* obj, MAPI_Color_ARGB const* color) noexcept
{
	obj->impl.SetBackgroundColor(*color);
}
