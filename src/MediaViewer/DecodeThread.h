#pragma once

#include "VideoViewer.h"
#include "Include/MediaApi.h"
#include <thread>

class VideoViewer::DecodeThread
{
	enum class Status : int
	{
		Idl = 0,
		Starting,
		Started,
	};

public:
	DecodeThread(ObjectQueue::SendingEnd&& frameSender, ObjectQueue::ReceivingEnd&& packetReceiver);
	~DecodeThread();

private:
	void WorkerThreadProc();
	void DoWorkerThreadLoop();
	void CreateResource();
	void DestroyResource();
	ThreadLoopStatus ProcessInterrupt(InterruptCode code);
	ThreadLoopStatus SendFrames();
	void ProcessInput(MediaObject* obj);
	void ProcessPacket(MediaPacketObject* packet);
	void ProcessMessage(MediaMessageObject* msg);
	void ProcessMediaDescriptor(MediaDescriptorObject* desc);
	void OnSessionStart();
	void OnStreamEnd();
	void SetStatus(Status status);

private:
	using MediaObjectList = std::list<Leo::ReferenceGuard<MediaObject>>;

	std::thread								m_thread;
	ObjectQueue::SendingEnd					m_frameSender;
	ObjectQueue::ReceivingEnd				m_packetReceiver;
	MediaObjectList							m_framesToSend;
	Status									m_status = Status::Idl;
	uint32_t								m_packetCount = 0;
};

