#include "pch.h"
#include "DX11RenderTarget.h"
#include "DX11Graphics.h"
#include "DirectXHelper.h"

namespace mapi {

DX11RenderTarget::DX11RenderTarget(DX11Graphics const& graphics)
	: m_graphics(graphics)
{
}

DX11RenderTarget::~DX11RenderTarget()
{
}

void DX11RenderTarget::FillColor(Color::ARGB const& color, Rect const* rect)
{
	DirectX::XMVECTORF32 d3dColor = CoverterToDirectXColor(color);

	// Just clear the backbuffer
	m_graphics.GetDeviceContext()->ClearRenderTargetView(
		m_graphics.GetRenderTargetView(), d3dColor);
}

void DX11RenderTarget::Present(Error& err)
{
	m_graphics.GetSwapChain()->Present(0, 0);
}

};
