#include "pch.h"
#include "Graphics.h"
#include "TargetView.h"
#include "RenderTarget.h"
#include "Diagnisis.h"

namespace mapi {

Graphics::~Graphics()
{
	VERIFY_CURRENT_THREAD_ID_IS(m_initializeThreadId);
}

void Graphics::Initialize(std::unique_ptr<TargetView>&& target_view, Error& err) noexcept
{
	if (m_view != nullptr)
	{
		err.code = MAPIERR_INVALID_CALL;
		return;
	}

	if (!::IsWindow(target_view->GetHwnd()))
	{
		err.code = MAPIERR_INVALID_ARG;
		return;
	}

	m_initializeThreadId = ::GetCurrentThreadId();
	m_view = std::move(target_view);
}

std::unique_ptr<RenderTarget> Graphics::BeginRendering(Error& err) noexcept
{
	VERIFY_CURRENT_THREAD_ID_IS(m_initializeThreadId);
	return std::unique_ptr<RenderTarget>();
}

void Graphics::EndRendering(std::unique_ptr<RenderTarget>&& target_) noexcept
{
	VERIFY_CURRENT_THREAD_ID_IS(m_initializeThreadId);
	auto target = std::move(target_);
}

};
