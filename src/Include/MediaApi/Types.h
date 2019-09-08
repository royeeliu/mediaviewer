#pragma once

#include "Config.h"
#include "Error.h"

static const int64_t MAPI_INVALID_TIMECODE = (int64_t)0x8000000000000000ULL;

enum class MAPI_MediaType : int
{
	Unknown = 0,
	Video = 1,
	Audio = 2,
};

struct MAPI_TargetView
{
	void* handle;
};

struct MAPI_Color_ARGB
{
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct MAPI_Rect
{
	int32_t left;
	int32_t bottom;
	int32_t right;
	int32_t top;
};

struct MAPI_Rational
{
	int32_t num;
	int32_t den;
};

struct MAPI_MediaSource;
struct MAPI_Presenter;
struct MAPI_StreamDescriptor;

struct MAPI_MediaDescriptor;
struct MAPI_MediaPacket;
struct MAPI_MediaFrame;

struct MAPI_Graphics;
struct MAPI_RenderTarget;

