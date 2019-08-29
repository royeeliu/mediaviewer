#pragma once

#include "VideoViewer.h"
#include "Include/MediaApi.h"
#include <thread>

class VideoViewer::RenderThread
{
public:
	RenderThread(ObjectQueue::ReceivingEnd&& frameChannel, HWND displayWindow);
	~RenderThread();

private:
	void WorkerThreadProc();
	void DoThreadLoop();
	void CreateResource();
	void DestroyResource();
	ThreadLoopStatus ProcessInterrupt(InterruptCode code);
	void Render();

private:
	std::thread						m_thread;
	ObjectQueue::ReceivingEnd		m_frameChannel;
	MAPI_TargetView					m_targetView{};
	MAPI_Graphics*					m_graphics{ nullptr };
	MAPI_Color_ARGB					m_bkgColor{ 255, 0, 0, 0 };
};

