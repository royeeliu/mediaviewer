#pragma once

#include "Types.h"
#include <DirectXMath.h>


namespace mapi {

DirectX::XMVECTORF32 CoverterToDirectXColor(Color::ARGB const& color)
{
	float r = color.r / 255.0f;
	float g = color.g / 255.0f;
	float b = color.b / 255.0f;
	float a = color.a / 255.0f;
	return DirectX::XMVECTORF32{ { { r, g, b, a } } };
}

}; // namesapce mapi
