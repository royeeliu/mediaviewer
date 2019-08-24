#include "pch.h"
#include "RenderTarget.h"
#include "DX11RenderTarget.h"

namespace mapi {

RenderTarget::RenderTarget(std::unique_ptr<DX11RenderTarget>&& target)
	: m_impl(std::move(target))
{
}

void RenderTarget::FillColor(Color::ARGB const& color, Rect const* rect)
{
	m_impl->FillColor(color, rect);
}

void RenderTarget::Present(Error& err)
{
	m_impl->Present(err);
}

};
