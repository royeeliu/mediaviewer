#include "framework.h"
#include "RenderThread.h"
#include "Common.h"
#include "Include/MediaApiHelper.h"

using namespace Leo;
using namespace Leo::Threading;
using namespace MediaApiHelper;

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
		ReferenceGuard<Referencable> frameObject;
		int interruptCode = 0;
		auto waitResult = m_frameChannel.Receive(*frameObject.GetAddress(), interruptCode);

		switch (waitResult)
		{
		case ObjectQueue::Status::Ok:
			ProcessInput(dynamic_cast<MediaObject*>(frameObject.Get()));
			break;

		case ObjectQueue::Status::Interrupted:
			status = ProcessInterrupt(static_cast<InterruptCode>(interruptCode));
			break;

		default: { _ASSERTE(false); }
		}
	}
}

void VideoViewer::RenderThread::CreateResource()
{
	m_graphics = std::shared_ptr<MAPI_Graphics>(
		MAPI_Graphics_Create(),
		[](MAPI_Graphics* ptr) { MAPI_Graphics_Destroy(&ptr); });

	MAPI_Error err{};
	MAPI_Graphics_Initialize(m_graphics.get(), &m_targetView, &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE(L"Initialize video render failed: error = %d", err.code);
	}
}

void VideoViewer::RenderThread::DestroyResource()
{
	m_videoRenderer.reset();
	m_graphics.reset();
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
		Repaint();
		break;

	default: { _ASSERTE(false); }
	}

	return ThreadLoopStatus::Continue;
}

void VideoViewer::RenderThread::ProcessInput(MediaObject* obj)
{
	switch (obj->type)
	{
	case MediaObjectType::Frame:
		ProcessFrame(dynamic_cast<MediaFrameObject*>(obj));
		break;

	case MediaObjectType::MediaDescriptor:
		ProcessMediaDescriptor(dynamic_cast<MediaDescriptorObject*>(obj));
		break;

	case MediaObjectType::Message:
		ProcessMessage(dynamic_cast<MediaMessageObject*>(obj));
		break;

	default: { ASSERT(!"Invalid type"); }
	}
}

void VideoViewer::RenderThread::ProcessFrame(MediaFrameObject* frame)
{
	MAPI_VideoRenderer_SendFrame(m_videoRenderer.get(), frame->frame.get());
}

void VideoViewer::RenderThread::ProcessMessage(MediaMessageObject* msg)
{
	switch (msg->msg)
	{
	case MediaMessage::None:
		break;
	case MediaMessage::SessionStart:
		OnSessionStart();
		break;
	case MediaMessage::StreamEnd:
		OnStreamEnd();
		break;
	default:
	{ ASSERT(false); }
	}
}

void VideoViewer::RenderThread::ProcessMediaDescriptor(MediaDescriptorObject* desc)
{
	PRINTF(".... Set media descriptor\n");
	_ASSERTE(m_status == Status::Starting);
	ResetRenderer(desc->media.get());
	SetStatus(Status::Started);
}

void VideoViewer::RenderThread::OnSessionStart()
{
	PRINTF(".... Segment start\n");
	SetStatus(Status::Starting);
}

void VideoViewer::RenderThread::OnStreamEnd()
{
}

void VideoViewer::RenderThread::SetStatus(Status status)
{
	m_status = status;
}

void VideoViewer::RenderThread::ResetRenderer(MAPI_MediaDescriptor* mediaDesc)
{
	auto renderer = std::shared_ptr<MAPI_VideoRenderer>(
		MAPI_VideoRenderer_Create(),
		[](MAPI_VideoRenderer* ptr) { MAPI_VideoRenderer_Destroy(&ptr); });

	MAPI_Error err{};
	MAPI_VideoRenderer_Initialize(renderer.get(), mediaDesc, m_graphics.get(), &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE(L"MAPI_VideoRenderer_Initialize failed: %s", FormatError(err));
		return;
	}

	MAPI_VideoRenderer_SetBackgroundColor(renderer.get(), &m_bkgColor);

	m_videoRenderer = std::move(renderer);
}

void VideoViewer::RenderThread::Repaint()
{
	MAPI_VideoRenderer_Repaint(m_videoRenderer.get());
}
