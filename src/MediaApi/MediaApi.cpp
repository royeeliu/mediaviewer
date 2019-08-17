#include "pch.h"
#include "Include/MediaApi.h"
#include "MediaSource.h"

using namespace mapi;

#define PRINTF printf

template<typename T>
struct MediaStruct
{
	using Class = typename T;
	Class impl;
};

struct MAPI_MediaSource : MediaStruct<MediaSource> {};


MEDIA_API MAPI_MediaSource* MAPI_MediaSource_Create() noexcept
{
	return new MAPI_MediaSource{};
}

MEDIA_API void MAPI_MediaSource_Destroy(MAPI_MediaSource* self) noexcept
{
	delete self;
}

MEDIA_API void MAPI_MediaSource_LoadFile(MAPI_MediaSource* self, const char* u8_url, MAPI_Error* err) noexcept
{
	MAPI_Error err_{ MAPI_NO_ERROR };
	err = (err != nullptr) ? err : &err_;
	self->impl.LoadFile(u8_url, *err);
}
