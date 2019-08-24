#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "DX11Graphics.h"

namespace mapi {

class DX11RenderTarget : Uncopyable
{
public:
	explicit
	DX11RenderTarget(DX11Graphics const& graphics);
	~DX11RenderTarget();

	void FillColor(Color::ARGB const& color, Rect const* rect);
	void Present(Error& err);

private:
	DX11Graphics const&			m_graphics;
	CComPtr<ID3D11Texture2D>	m_backBuffer;
};

};
