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

struct MAPI_MediaSource : MediaStruct<MediaSource> {};
struct MAPI_Presenter : MediaStruct<Presenter> {};
struct MAPI_Graphics : MediaStruct<Graphics> {};
struct MAPI_RenderTarget : MediaStruct<std::unique_ptr<RenderTarget>> {};


MEDIA_API MAPI_MediaSource* MAPI_MediaSource_Create() noexcept
{
	return new MAPI_MediaSource{};
}

MEDIA_API void MAPI_MediaSource_Destroy(MAPI_MediaSource* self) noexcept
{
	SafeDelete(&self);
}

MEDIA_API void MAPI_MediaSource_LoadFile(MAPI_MediaSource* self, const char* u8_url, MAPI_Error* err) noexcept
{
	MAPI_Error err_{ MAPI_NO_ERROR };
	err = (err != nullptr) ? err : &err_;
	self->impl.LoadFile(u8_url, *err);
}

MEDIA_API MAPI_Presenter* MAPI_Presenter_Create() noexcept
{
	return new MAPI_Presenter{};
}

MEDIA_API void MAPI_Presenter_Destroy(MAPI_Presenter* self) noexcept
{
	SafeDelete(&self);
}

MEDIA_API void MAPI_Presenter_Initialize(MAPI_Presenter* self, MAPI_TargetView const* view, MAPI_Error* err) noexcept
{
	MAPI_Error err_{ MAPI_NO_ERROR };
	err = (err != nullptr) ? err : &err_;
	self->impl.Initialize(std::make_unique<TargetView>(view), *err);
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

MEDIA_API MAPI_RenderTarget* MAPI_Graphics_BeginRendering(MAPI_Graphics* handle, MAPI_Error* err) noexcept
{
	MAPI_Error err_{ MAPI_NO_ERROR };
	err = (err != nullptr) ? err : &err_;
	auto impl = handle->impl.BeginRendering(*err);

	if (err->code != MAPI_NO_ERROR)
	{
		return nullptr;
	}

	auto result = new MAPI_RenderTarget{};
	result->impl = std::move(impl);

	return result;
}

MEDIA_API void MAPI_Graphic_EndRendering(MAPI_Graphics* handle, MAPI_RenderTarget** target) noexcept
{
	handle->impl.EndRendering(std::move((*target)->impl));
	SafeDelete(target);
}

MEDIA_API void MAPI_RenderTarget_FillColor(MAPI_RenderTarget* handle, MAPI_Color_ARGB const* color, MAPI_Rect const* rect)
{
	handle->impl->FillColor(*color, rect);
}

