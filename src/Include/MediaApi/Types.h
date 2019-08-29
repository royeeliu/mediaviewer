#pragma once

#include "Config.h"

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

