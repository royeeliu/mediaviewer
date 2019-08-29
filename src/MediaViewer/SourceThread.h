#pragma once

#include "VideoViewer.h"
#include "Include/MediaApi.h"
#include <thread>

class VideoViewer::SourceThread
{
public:
	SourceThread(ObjectQueue::SendingEnd&& packetChannel, CommandChannel::ReceivingEnd&& commandChannel);
	~SourceThread();

private:
	void WorkerThreadProc();
	void DoWorkerThreadLoop();
	void CreateResource();
	void DestroyResource();
	ThreadLoopStatus ProcessCommand(CommandObject& cmd);
	void LoadFile(std::wstring const& fileName);

private:
	std::thread							m_thread;
	ObjectQueue::SendingEnd				m_packetChannel;
	CommandChannel::ReceivingEnd		m_commandChannel;
	bool								m_started = false;
	MAPI_MediaSource*					m_mediaSource = nullptr;
};
