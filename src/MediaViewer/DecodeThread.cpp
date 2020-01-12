#include "framework.h"
#include "DecodeThread.h"
#include "Common.h"
#include "Dispatcher.h"
#include "Include/MediaApiHelper.h"

using namespace Leo;
using namespace MediaApiHelper;

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
	m_decoder.reset();
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
	if (!m_decoder)
	{
		return;
	}

	MAPI_Error err{};
	MAPI_Decoder_SendPacket(m_decoder.get(), packet->packet.get(), &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE(L"MAPI_Decoder_SendPacket failed: %s", FormatError(err).c_str());
		return;
	}

	auto frame = std::shared_ptr<MAPI_MediaFrame>(
		MAPI_Decoder_ReceiveFrame(m_decoder.get(), &err),
		[](MAPI_MediaFrame* ptr) { MAPI_MediaFrame_Destroy(&ptr); });

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE(L"MAPI_Decoder_ReceiveFrame failed: %s", FormatError(err).c_str());
		return;
	}

	if (frame != nullptr)
	{
		PushObjectToSendingList(new MediaFrameObject{ std::move(frame) });
	}
}

void VideoViewer::DecodeThread::ProcessMessage(MediaMessageObject* msg)
{
	switch (msg->msg)
	{
	case MediaMessage::None:
		break;
	case MediaMessage::SessionStart:
		OnSessionStart(msg);
		break;
	case MediaMessage::StreamEnd:
		OnStreamEnd(msg);
		break;
	}
}

void VideoViewer::DecodeThread::ProcessMediaDescriptor(MediaDescriptorObject* desc)
{
	PRINTF(".... Set media descriptor\n");
	_ASSERTE(m_status == Status::Starting);

	CreateDecoder(desc->media.get());
	SetStatus(Status::Started);

	auto outputDesc = std::shared_ptr<MAPI_MediaDescriptor>(
		MAPI_Decoder_GetOutputMediaDescriptor(m_decoder.get()),
		[](MAPI_MediaDescriptor* ptr) { MAPI_MediaDescriptor_Destroy(&ptr); });
	PushObjectToSendingList(new MediaDescriptorObject{ std::move(outputDesc) });
}

void VideoViewer::DecodeThread::OnSessionStart(MediaMessageObject* msg)
{
	PRINTF(".... Segment start\n");
	SetStatus(Status::Starting);
	PushObjectToSendingList(Leo::AddReference(msg));
}

void VideoViewer::DecodeThread::OnStreamEnd(MediaMessageObject* msg)
{
	PushObjectToSendingList(Leo::AddReference(msg));
}

void VideoViewer::DecodeThread::SetStatus(Status status)
{
	m_status = status;
}

void VideoViewer::DecodeThread::CreateDecoder(MAPI_MediaDescriptor* mediaDesc)
{
	auto decoder = std::shared_ptr<MAPI_Decoder>(
		MAPI_Decoder_Create(),
		[](MAPI_Decoder* ptr) { MAPI_Decoder_Destroy(&ptr); });

	MAPI_Error err{};
	MAPI_Decoder_Initialize(decoder.get(), mediaDesc, nullptr, &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE(L"MAPI_Decoder_Initialize failed: %s", FormatError(err).c_str());
		return;
	}

	m_decoder = decoder;
}

void VideoViewer::DecodeThread::PushObjectToSendingList(MediaObject*&& object)
{
	m_framesToSend.push_back(Leo::MakeReferenceGuard(std::move(object)));
}
