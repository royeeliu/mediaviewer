#pragma once

#include "Types.h"
#include "Uncopyable.h"

namespace mapi {

class RenderTarget : Uncopyable
{
public:
	RenderTarget() = default;
	~RenderTarget() = default;

	void FillColor(Color::ARGB const& color, Rect const* rect);
};

};
