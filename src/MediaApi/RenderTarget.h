#pragma once

#include "Types.h"
#include "Uncopyable.h"

namespace mapi {

class RenderTarget : Uncopyable
{
public:
	explicit
	RenderTarget(std::unique_ptr<DX11RenderTarget>&& target);
	~RenderTarget() = default;

	void FillColor(Color::ARGB const& color, Rect const* rect);
	void Present(Error& err);

private:
	std::unique_ptr<DX11RenderTarget> m_impl;
};

};
