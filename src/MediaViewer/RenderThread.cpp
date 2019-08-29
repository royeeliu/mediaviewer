#include "framework.h"
#include "RenderThread.h"
#include "Common.h"

using namespace Leo;
using namespace Leo::Threading;

VideoViewer::RenderThread::RenderThread(ObjectQueue::ReceivingEnd&& frameChannel, HWND displayWindow)
	: m_frameChannel(std::move(frameChannel))
{
	m_targetView.handle = (HWND)displayWindow;
	m_thread = std::thread([this] { WorkerThreadProc(); });
}

VideoViewer::RenderThread::~RenderThread()
{
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void VideoViewer::RenderThread::WorkerThreadProc()
{
	CreateResource();
	DoThreadLoop();
	DestroyResource();
}

void VideoViewer::RenderThread::DoThreadLoop()
{
	ThreadLoopStatus status = ThreadLoopStatus::Continue;

	while (status != ThreadLoopStatus::Exit)
	{
		ReferenceGuard<Referencable> object;
		int interruptCode = 0;
		auto waitResult = m_frameChannel.Receive(*object.GetAdress(), interruptCode);

		switch (waitResult)
		{
		case WaitResult::Ok:
			break;

		case WaitResult::Interrupted:
			status = ProcessInterrupt(static_cast<InterruptCode>(interruptCode));
			break;

		default: { _ASSERTE(false); }
		}
	}
}

void VideoViewer::RenderThread::CreateResource()
{
	m_graphics = MAPI_Graphics_Create();

	MAPI_Error err{};
	MAPI_Graphics_Initialize(m_graphics, &m_targetView, &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE(L"Initialize video render failed: error = %d", err.code);
	}
}

void VideoViewer::RenderThread::DestroyResource()
{
	if (m_graphics != nullptr)
	{
		MAPI_Graphics_Destroy(&m_graphics);
	}
}

VideoViewer::ThreadLoopStatus VideoViewer::RenderThread::ProcessInterrupt(InterruptCode code)
{
	switch (code)
	{
	case InterruptCode::None:
		break;

	case InterruptCode::SendCommand:
		break;

	case InterruptCode::Shutdown:
		return ThreadLoopStatus::Exit;

	case InterruptCode::Repaint:
		Render();
		break;

	default: { _ASSERTE(false); }
	}

	return ThreadLoopStatus::Continue;
}

void VideoViewer::RenderThread::Render()
{
	MAPI_Error err{};
	MAPI_RenderTarget* target = MAPI_Graphics_BeginRender(m_graphics, &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE("MAPI_Graphics_BeginRender failed: {%d, %d, %s}", err.code, err.detail, err.message);
		return;
	}

	MAPI_RenderTarget_FillColor(target, &m_bkgColor, nullptr);
	MAPI_Graphic_EndRender(m_graphics, &target, &err);
}

