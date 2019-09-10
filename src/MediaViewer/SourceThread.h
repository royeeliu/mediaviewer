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
	ThreadLoopStatus ProcessInterrupt(InterruptCode code);
	ThreadLoopStatus SendPackets();
	void LoadFile(std::wstring const& fileName);
	void ReadPacket();

private:
	using MediaObjectList = std::list<Leo::ReferenceGuard<MediaObject>>;

	std::thread								m_thread;
	ObjectQueue::SendingEnd					m_packetChannel;
	CommandChannel::ReceivingEnd			m_commandChannel;
	MediaObjectList							m_packetsToSend;
	bool									m_started = false;
	std::shared_ptr<MAPI_MediaSource>		m_mediaSource;
	std::shared_ptr<MAPI_MediaDescriptor>	m_videoMediaDesc;
	uint32_t								m_videoStreamIndex = 0;
	uint32_t								m_videoPacketCount = 0;

};
