#pragma once

#include "framework.h"
#include "Include/MediaApi/Types.h"

namespace mapi {

using Error = MAPI_Error;
using ERROR_CODE = MAPI_ERROR_CODE;
using Rect = MAPI_Rect;
using MediaType = MAPI_MediaType;
using Rational = MAPI_Rational;

namespace Color {
	using ARGB = MAPI_Color_ARGB;
}

class TargetView;
class RenderTarget;
class DX11Graphics;
class DX11RenderTarget;

class MediaPacket;


} // End of namespace


