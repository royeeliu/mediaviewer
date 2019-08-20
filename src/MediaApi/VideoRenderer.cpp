#include "pch.h"
#include "VideoRenderer.h"
#include "ImageView.h"
#include "Diagnisis.h"
#include <mfapi.h>

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "mf.lib")

namespace mapi {

VideoRenderer::VideoRenderer()
{
}

VideoRenderer::~VideoRenderer()
{
	VERIFY_CURRENT_THREAD_ID_IS(m_initializeThreadId);

	if (m_mfStartuped)
	{
		HRESULT hr = ::MFShutdown();
		ASSERT(SUCCEEDED(hr));
		m_mfStartuped = false;
	}

	if (m_comInitialized)
	{
		::CoUninitialize();
		m_comInitialized = false;
	}
}

void VideoRenderer::Initialize(std::unique_ptr<ImageView>&& view, Error& err)
{
	if (m_view != nullptr)
	{
		err.code = MAPIERR_INVALID_CALL;
		return;
	}

	if (!::IsWindow(view->GetHwnd()))
	{
		err.code = MAPIERR_INVALID_ARG;
		return;
	}

	m_initializeThreadId = ::GetCurrentThreadId();
	m_view = std::move(view);

	HRESULT hr = ::CoInitializeEx(nullptr, 0);
	RETURN_VOID_IF(!CHECK_COM_RESULT("CoInitializeEx", hr, err));
	m_comInitialized = true;

	hr = ::MFStartup(MF_VERSION);
	RETURN_VOID_IF(!CHECK_COM_RESULT("MFStartup", hr, err));
	m_mfStartuped = true;

	hr = ::MFCreateVideoRenderer(IID_PPV_ARGS(&m_evr));
	RETURN_VOID_IF(!CHECK_COM_RESULT("MFCreateVideoRenderer", hr, err));
}

};
