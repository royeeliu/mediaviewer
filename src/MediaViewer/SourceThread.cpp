#include "framework.h"
#include "SourceThread.h"
#include "Common.h"
#include "Dispatcher.h"
#include "Include/Helper/StringHelper.h"
#include "Include/MediaApiHelper.h"

using namespace StringHelper;
using namespace MediaApiHelper;

VideoViewer::SourceThread::SourceThread(ObjectQueue::SendingEnd&& packetChannel, CommandChannel::ReceivingEnd&& commandChannel)
	: m_packetChannel(std::move(packetChannel))
	, m_commandChannel(std::move(commandChannel))
{
	m_thread = std::thread([this] { WorkerThreadProc(); });
}

VideoViewer::SourceThread::~SourceThread()
{
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void VideoViewer::SourceThread::WorkerThreadProc()
{
	CreateResource();
	DoWorkerThreadLoop();
	DestroyResource();
}

void VideoViewer::SourceThread::DoWorkerThreadLoop()
{
	ThreadLoopStatus status = ThreadLoopStatus::Continue;

	while (status != ThreadLoopStatus::Exit)
	{
		if (!m_started || status == ThreadLoopStatus::WaitCommand)
		{
			CommandObject cmd = m_commandChannel.Receive();
			status = ProcessCommand(cmd);
			continue;
		}

		if (!m_packetsToSend.empty())
		{
			status = SendPackets();

			if (status != ThreadLoopStatus::Continue)
			{
				continue;
			}
		}

		ASSERT(m_started);
		ReadPacket();
	}
}

void VideoViewer::SourceThread::CreateResource()
{
	_ASSERTE(m_mediaSource == nullptr);
	m_mediaSource = std::shared_ptr<MAPI_MediaSource>(
		MAPI_MediaSource_Create(),
		[](MAPI_MediaSource* ptr) { MAPI_MediaSource_Destroy(&ptr); });
}

void VideoViewer::SourceThread::DestroyResource()
{
	m_videoMediaDesc.reset();
	m_mediaSource.reset();
}

VideoViewer::ThreadLoopStatus VideoViewer::SourceThread::ProcessCommand(CommandObject& cmd)
{
	auto status = ThreadLoopStatus::Continue;

	switch (cmd->cmd)
	{
	case Command::ID::LoadFile:
		LoadFile(dynamic_cast<LoadFileCommand*>(cmd.Get())->fileName);
		break;

	default: { _ASSERTE(false); }
	}

	return status;
}

VideoViewer::ThreadLoopStatus VideoViewer::SourceThread::ProcessInterrupt(InterruptCode code)
{
	switch (code)
	{
	case InterruptCode::SendCommand:
		return ThreadLoopStatus::WaitCommand;
	case InterruptCode::Shutdown:
		return ThreadLoopStatus::Exit;
	default:
		ASSERT(!"Invalid code");
		return ThreadLoopStatus::Continue;
	}
}

VideoViewer::ThreadLoopStatus VideoViewer::SourceThread::SendPackets()
{
	ThreadLoopStatus status = ThreadLoopStatus::Continue;

	while (!m_packetsToSend.empty())
	{
		auto packet = m_packetsToSend.front();

		int code = static_cast<int>(InterruptCode::None);
		auto channelStatus = m_packetChannel.Send(packet.Get(), code);

		if (channelStatus == ObjectQueueStatus::Ok)
		{
			m_packetsToSend.pop_front();
		}
		else if (channelStatus == ObjectQueueStatus::Interrupted)
		{
			status = ProcessInterrupt(static_cast<InterruptCode>(code));
			break;
		}
		else
		{
			ASSERT(false);
		}
	}

	return status;
}

void VideoViewer::SourceThread::LoadFile(std::wstring const& fileName)
{
	_ASSERTE(m_mediaSource != nullptr);
	MAPI_Error err{};
	MAPI_MediaSource_LoadFile(m_mediaSource.get(), W2U8(fileName).c_str(), &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE(L"MAPI_MediaSource_LoadFile failed: %s, %s",
			FormatError(err).c_str(),
			fileName.c_str());
		return;
	}

	WPRINTF(L"File opened: %s\n", fileName.c_str());
	uint32_t const streamCount = MAPI_MediaSource_GetStreamCount(m_mediaSource.get());

	for (uint32_t i = 0; i < streamCount; i++)
	{
		MAPI_StreamDescriptor* stream = MAPI_MediaSource_GetStreamDescriptor(m_mediaSource.get(), i);
		uint32_t id = MAPI_StreamDescriptor_GetId(stream);
		MAPI_MediaType type = MAPI_StreamDescriptor_GetMediaType(stream);
		MAPI_StreamDescriptor_Destroy(&stream);

		WPRINTF(L"##[%d] Stream %d: %s\n", i, id, U8ToW(MediaApiHelper::MediaTypeName(type)).c_str());
	}

	m_videoStreamIndex = MAPI_MediaSource_FindBestStream(m_mediaSource.get(), MAPI_MediaType::Video);

	MAPI_StreamDescriptor* videoStream = MAPI_MediaSource_GetStreamDescriptor(m_mediaSource.get(), m_videoStreamIndex);
	MAPI_Rational timebase{};
	MAPI_StreamDescriptor_GetTimebase(videoStream, &timebase);

	WPRINTF(L"best video stream: %d, timebase{%d,%d}\n", m_videoStreamIndex, timebase.num, timebase.den);

	m_videoMediaDesc = std::shared_ptr<MAPI_MediaDescriptor>(
		MAPI_StreamDescriptor_GetMediaDescriptor(videoStream),
		[](MAPI_MediaDescriptor* ptr) { MAPI_MediaDescriptor_Destroy(&ptr); });
	MAPI_StreamDescriptor_Destroy(&videoStream);

	auto msgObj = new MediaMessageObject{ MediaMessage::SessionStart };
	auto mediaDescObj = new MediaDescriptorObject{ m_videoMediaDesc };
	m_packetsToSend.push_back(Leo::MakeReferenceGuard(static_cast<MediaObject*>(msgObj)));
	m_packetsToSend.push_back(Leo::MakeReferenceGuard(static_cast<MediaObject*>(mediaDescObj)));

	m_started = true;
}

void VideoViewer::SourceThread::ReadPacket()
{
	MAPI_Error err{};
	auto packet = std::shared_ptr<MAPI_MediaPacket>(
		MAPI_MediaSource_ReadPacket(m_mediaSource.get(), &err),
		[](MAPI_MediaPacket* ptr) { MAPI_MediaPacket_Destroy(&ptr); });

	if (err.code != MAPI_NO_ERROR)
	{
		ASSERT(packet == nullptr);
		SHOW_ERROR_MESSAGE(L"MAPI_MediaSource_ReadPacket failed: %s", FormatError(err).c_str());
		m_started = false;
		return;
	}

	int streamIndex = MAPI_MediaPacket_GetStreamIndex(packet.get());

	if (streamIndex != (int)m_videoStreamIndex)
	{
		return;
	}

	int64_t pts = MAPI_MediaPacket_GetPts(packet.get());
	int64_t dts = MAPI_MediaPacket_GetDts(packet.get());
	UNREFERENCED_PARAMETER(pts);
	UNREFERENCED_PARAMETER(dts);
	//WPRINTF(L"\r[%u]pts: %lld, \tdts: %lld                                      ", m_videoPacketCount, pts, dts);
	m_videoPacketCount++;

	m_packetsToSend.push_back(
		Leo::MakeReferenceGuard(static_cast<MediaObject*>(new MediaPacketObject{ packet })));
}
