#include "pch.h"
#include "Include/MediaApi.h"
#include "MediaSource.h"
#include "Presenter.h"
#include "TargetView.h"
#include "Graphics.h"
#include "RenderTarget.h"

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
struct MAPI_Presenter : MediaStruct<Presenter> {};
struct MAPI_Graphics : MediaStruct<Graphics> {};
struct MAPI_RenderTarget : MediaStruct<RenderTarget*> {};
struct MAPI_SourceStream : MediaStruct<SourceStream*> {};

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
	MAPI_Error err_{ MAPI_NO_ERROR };
	err = (err != nullptr) ? err : &err_;
	obj->impl.LoadFile(u8_url, *err);
}

MEDIA_API uint32_t MAPI_MediaSource_GetStreamCount(MAPI_MediaSource* obj) noexcept
{
	return obj->impl.GetStreamCount();
}

MEDIA_API MAPI_SourceStream* MAPI_MediaSource_GetStream(MAPI_MediaSource* obj, uint32_t index) noexcept
{
	auto stream = new MAPI_SourceStream{};
	stream->impl = obj->impl.GetStream(index);
	return stream;
}

MEDIA_API void MAPI_SourceStream_Destroy(MAPI_SourceStream** obj) noexcept
{
	SafeDelete(obj);
}

MEDIA_API MAPI_MediaType MAPI_SourceStream_GetMediaType(MAPI_SourceStream* obj) noexcept
{
	return obj->impl->GetMediaType();
}

MEDIA_API uint32_t MAPI_SourceStream_GetId(MAPI_SourceStream* obj) noexcept
{
	return obj->impl->GetId();
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
	MAPI_Error err_{ MAPI_NO_ERROR };
	err = (err != nullptr) ? err : &err_;
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
	MAPI_Error err_{ MAPI_NO_ERROR };
	err = (err != nullptr) ? err : &err_;
	handle->impl.Initialize(std::make_unique<TargetView>(target_view), *err);
}

MEDIA_API MAPI_RenderTarget* MAPI_Graphics_BeginRender(MAPI_Graphics* handle, MAPI_Error* err) noexcept
{
	MAPI_Error err_{ MAPI_NO_ERROR };
	err = (err != nullptr) ? err : &err_;
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
	MAPI_Error err_{ MAPI_NO_ERROR };
	err = (err != nullptr) ? err : &err_;
	handle->impl.EndRendering((*target)->impl, *err);
	SafeDelete(target);
}

MEDIA_API void MAPI_RenderTarget_FillColor(MAPI_RenderTarget* handle, MAPI_Color_ARGB const* color, MAPI_Rect const* rect)
{
	handle->impl->FillColor(*color, rect);
}

