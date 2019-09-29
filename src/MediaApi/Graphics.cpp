#include "pch.h"
#include "Graphics.h"
#include "TargetView.h"
#include "RenderTarget.h"
#include "DX11Graphics.h"
#include "DX11RenderTarget.h"
#include "VideoProcessor.h"
#include "Common.h"

namespace mapi {

Graphics::Graphics()
{
}

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
	m_graphics = std::make_unique<DX11Graphics>();

	do
	{
		m_graphics->Initialize(m_view->GetHwnd(), err);
		BREAK_IF(err.code != MAPI_NO_ERROR);

	} while (0);

	if (err.code != MAPI_NO_ERROR)
	{
		Clear();
	}
}

RenderTarget* Graphics::BeginRendering(Error& err) noexcept
{
	VERIFY_CURRENT_THREAD_ID_IS(m_initializeThreadId);
	return m_renderTarget.get();
}

void Graphics::EndRendering(RenderTarget*& target, Error& err) noexcept
{
	VERIFY_CURRENT_THREAD_ID_IS(m_initializeThreadId);
	ENSURE(target != nullptr);
	ENSURE(target == m_renderTarget.get());
	target = nullptr;
	m_renderTarget->Present(err);
}

std::unique_ptr<VideoProcessor> Graphics::CreateProcessor(MediaDescriptor const& media, Error& err)
{
	auto processor = std::make_unique<VideoProcessor>(*m_graphics);
	//processor->Initialize(media, err);

	//if (err.code != MAPI_NO_ERROR)
	//{
	//	return std::unique_ptr<VideoProcessor>{};
	//}

	return processor;
}

void Graphics::Clear()
{
	m_renderTarget.reset();
	m_graphics.reset();
	m_view.reset();
}

};
