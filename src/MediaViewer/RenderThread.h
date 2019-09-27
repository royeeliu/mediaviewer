#pragma once

#include "VideoViewer.h"
#include "Include/MediaApi.h"
#include <thread>

class VideoViewer::RenderThread
{
	enum class Status : int
	{
		Idl = 0,
		Starting,
		Started,
	};

public:
	RenderThread(ObjectQueue::ReceivingEnd&& frameChannel, HWND displayWindow);
	~RenderThread();

private:
	void WorkerThreadProc();
	void DoThreadLoop();
	void CreateResource();
	void DestroyResource();
	ThreadLoopStatus ProcessInterrupt(InterruptCode code);
	void ProcessInput(MediaObject* obj);
	void ProcessFrame(MediaFrameObject* frame);
	void ProcessMessage(MediaMessageObject* msg);
	void ProcessMediaDescriptor(MediaDescriptorObject* desc);

	void OnSessionStart();
	void OnStreamEnd();
	void SetStatus(Status status);
	void ResetRenderer(MAPI_MediaDescriptor* mediaDesc);
	void Repaint();

private:
	std::thread							m_thread;
	ObjectQueue::ReceivingEnd			m_frameChannel;
	Status								m_status{ Status::Idl };
	MAPI_TargetView						m_targetView{};
	MAPI_Color_ARGB						m_bkgColor{ 255, 0, 0, 0 };
	std::shared_ptr<MAPI_Graphics>		m_graphics;
	std::shared_ptr<MAPI_VideoRenderer>	m_videoRenderer;
};

