#include "framework.h"
#include "DecodeThread.h"
#include "Common.h"
#include "Dispatcher.h"

using namespace Leo;

VideoViewer::DecodeThread::DecodeThread(ObjectQueue::SendingEnd&& frameSender, ObjectQueue::ReceivingEnd&& packetReceiver)
	: m_frameSender(std::move(frameSender))
	, m_packetReceiver(std::move(packetReceiver))
{
	m_thread = std::thread([this] { WorkerThreadProc(); });
}

VideoViewer::DecodeThread::~DecodeThread()
{
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void VideoViewer::DecodeThread::WorkerThreadProc()
{
	CreateResource();
	DoWorkerThreadLoop();
	DestroyResource();
}

void VideoViewer::DecodeThread::DoWorkerThreadLoop()
{
	ThreadLoopStatus status = ThreadLoopStatus::Continue;

	while (status != ThreadLoopStatus::Exit)
	{
		ReferenceGuard<Referencable> packetObj;
		int code = static_cast<int>(InterruptCode::None);
		auto receiverStatus = m_packetReceiver.Receive(*packetObj.GetAddress(), code);

		if (receiverStatus == ObjectQueueStatus::Interrupted)
		{
			status = ProcessInterrupt(static_cast<InterruptCode>(code));
			continue;
		}

		ASSERT(receiverStatus == ObjectQueueStatus::Ok);
		ASSERT(packetObj != nullptr);
		ProcessInput(dynamic_cast<MediaObject*>(packetObj.Get()));

		if (!m_framesToSend.empty())
		{
			status = SendFrames();
		}
	}
}

void VideoViewer::DecodeThread::CreateResource()
{
}

void VideoViewer::DecodeThread::DestroyResource()
{
}

VideoViewer::ThreadLoopStatus VideoViewer::DecodeThread::ProcessInterrupt(InterruptCode code)
{
	switch (code)
	{
	case InterruptCode::Shutdown:
		return ThreadLoopStatus::Exit;
	default:
		ASSERT(!"Invalid code");
		return ThreadLoopStatus::Continue;
	}
}

VideoViewer::ThreadLoopStatus VideoViewer::DecodeThread::SendFrames()
{
	ThreadLoopStatus loopStatus = ThreadLoopStatus::Continue;

	while (!m_framesToSend.empty())
	{
		auto frame = m_framesToSend.front();

		int code = static_cast<int>(InterruptCode::None);
		auto senderStatus = m_frameSender.Send(frame.Get(), code);

		if (senderStatus == ObjectQueueStatus::Ok)
		{
			m_framesToSend.pop_front();
		}
		else if (senderStatus == ObjectQueueStatus::Interrupted)
		{
			loopStatus = ProcessInterrupt(static_cast<InterruptCode>(code));
			break;
		}
		else
		{
			ASSERT(false);
		}
	}

	return loopStatus;
}

void VideoViewer::DecodeThread::ProcessInput(MediaObject* obj)
{
	switch (obj->type)
	{
	case MediaObjectType::Packet:
		ProcessPacket(dynamic_cast<MediaPacketObject*>(obj));
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

void VideoViewer::DecodeThread::ProcessPacket(MediaPacketObject* packet)
{
	m_packetCount++;
	PRINTF("\rpackets: %d", m_packetCount);
}

void VideoViewer::DecodeThread::ProcessMessage(MediaMessageObject* msg)
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
	}
}

void VideoViewer::DecodeThread::ProcessMediaDescriptor(MediaDescriptorObject* desc)
{
	PRINTF(".... Set media descriptor\n");
	SetStatus(Status::Started);
}

void VideoViewer::DecodeThread::OnSessionStart()
{
	PRINTF(".... Segment start\n");
	SetStatus(Status::Starting);
}

void VideoViewer::DecodeThread::OnStreamEnd()
{
}

void VideoViewer::DecodeThread::SetStatus(Status status)
{
	m_status = status;
}
