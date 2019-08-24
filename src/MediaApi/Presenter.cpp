
#include "pch.h"
#include "Presenter.h"
#include "TargetView.h"
#include "Common.h"
#include <mfapi.h>

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "mf.lib")

namespace mapi {

Presenter::Presenter()
{
}

Presenter::~Presenter()
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

void Presenter::Initialize(std::unique_ptr<TargetView>&& view, Error& err)
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

	hr = m_evr->InitializeRenderer(nullptr, nullptr);
	RETURN_VOID_IF(!CHECK_COM_RESULT("InitializeRenderer", hr, err));

	CComQIPtr<IMFGetService> getService(m_evr);
	hr = getService->GetService(MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&m_displayCtrl));
	RETURN_VOID_IF(!CHECK_COM_RESULT("GetService", hr, err));

	hr = getService->GetService(MR_VIDEO_MIXER_SERVICE, IID_PPV_ARGS(&m_videoProcessor));
	RETURN_VOID_IF(!CHECK_COM_RESULT("GetService", hr, err));

	m_videoProcessor->SetBackgroundColor(RGB(0, 255, 0));

	hr = m_displayCtrl->SetVideoWindow(m_view->GetHwnd());
	RETURN_VOID_IF(!CHECK_COM_RESULT("SetVideoWindow", hr, err));

	hr = m_displayCtrl->SetBorderColor(RGB(255, 0, 0));
	ASSERT(SUCCEEDED(hr));

	RECT rect{};
	::GetClientRect(m_view->GetHwnd(), &rect);
	hr = m_displayCtrl->SetVideoPosition(nullptr, &rect);
	ASSERT(SUCCEEDED(hr));

	m_displayCtrl->RepaintVideo();
}

void Presenter::RepaintVideo()
{
	if (m_displayCtrl)
	{
		m_displayCtrl->RepaintVideo();
	}
}

};
