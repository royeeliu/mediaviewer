#pragma once

#include "framework.h"
#include "Include/MediaApi/Types.h"

namespace mapi {

using ERROR_CODE = MAPI_ERROR_CODE;
using MediaType = MAPI_MediaType;
using Error = MAPI_Error;
using Rect = MAPI_Rect;
using Rational = MAPI_Rational;

namespace Color {
	using ARGB = MAPI_Color_ARGB;
}

class TargetView;
class RenderTarget;
class DX11Graphics;
class DX11RenderTarget;
class Graphics;
class MediaDescriptor;

class MediaPacket;
class MediaFrame;


} // End of namespace


