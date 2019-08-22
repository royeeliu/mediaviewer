#pragma once

#include "Config.h"

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

